#include<unistd.h>
#include<sys/fcntl.h>
#include<errno.h>
#include<stdio.h>
#include<sys/wait.h>
void wait_child(pid_t child_pid)
{
    int status=0;
    while(1)
    {
        //        等待子进程结束，如果等待的进程不是立即可用，waitpid不阻塞且返回0
        if(waitpid(child_pid,&status,WNOHANG)!=0)
        {
            //WIFEXITED(status);
            break;
        }
    };
}


/*文件记录锁说明：
 * 1.记录锁分为共享读锁和独占性写锁，锁的兼容性规则如下
 * 如果在一个给定的字节上已经有一把或多把读锁，则不能在该字节上再加写锁;
 * 如果在一个给定字节上已经有一把写锁，则不能再对它加任何读锁和写锁;
 * 2.如果同一个进程提出多个锁请求则后面的锁替换前面的锁;
 * 3.加读锁时描述符必须是读打开，加写锁时描述符必须是写打开;
 */
#define FILE_MODE S_IRUSR|S_IWUSR
static int log_reg(int fd,int cmd,int type,off_t offset,int whence,off_t len)
{
    struct flock flockptr;
    flockptr.l_type=type;//F_RDLCK,F_WRLCK,F_UNLCK
    flockptr.l_whence=whence;//SEEK_SET从文件起始位置;SEEK_CUR从文件当前位置;SEEK_END从文件末尾
    flockptr.l_start=offset;//偏移量
    flockptr.l_len=len;//区域字节长度，为0表示可以扩展到最大偏移量
    return fcntl(fd,cmd,&flockptr);
}
#define READ_LOCK(fd,offset,whence,len) log_reg(fd,F_SETLK,F_RDLCK,offset,whence,len);
#define READ_LOCKW(fd,offset,whence,len) log_reg(fd,F_SETLKW,F_RDLCK,offset,ehence,len);
#define WRITE_LOCK(fd,offset,whence,len) log_reg(fd,F_SETLK,F_WRLCK,offset,whence,len);
#define WRITE_LOCKW(fd,offset,whence,len) log_reg(fd,F_SETLKW,F_WRLCK,offset,whence,len);
#define UNLOCK(fd,offset,whence,len) log_reg(fd,F_SETLK,F_UNLCK,offset,whence,len);
static pid_t lock_test(int fd,int type,off_t offset,int whence,off_t len)
{
    struct flock flockptr;
    flockptr.l_type=type;
    flockptr.l_whence=whence;
    flockptr.l_start=offset;
    flockptr.l_len=len;
    if(fcntl(fd,F_GETLK,&flockptr)<0)
    {
        perror("fcntl");
    }
    if(flockptr.l_type==F_UNLCK)
    {
        return 0;
    }
    return flockptr.l_pid;
}


void father_process()
{
    int fd=open("./test_record.txt",O_RDWR|O_CREAT|O_NONBLOCK,FILE_MODE);
    if(fd<0)
    {
        perror("open");
    }
    if(!lock_test(fd,F_WRLCK,0,SEEK_SET,0))//测试该文件是否可以加一把写锁
    {
        WRITE_LOCK(fd,0,SEEK_SET,0);//加一把写锁
        write(fd,"12345",5);
        //        UNLOCK(fd,0,SEEK_SET,0);
    }
    printf("father process\n");
    sleep(30);

}

void child_process_rclock()
{
    sleep(1);
    int fd=open("./test_record.txt",O_RDWR|O_CREAT|O_NONBLOCK,FILE_MODE);
    if(fd<0)
    {
        perror("open");
    }
    if(!lock_test(fd,F_RDLCK,0,SEEK_SET,0))//测试该文件是否可以加一把读锁
    {
       READ_LOCK(fd,0,SEEK_SET,0);//加一把读锁
       write(fd,"helloworld",10);
       UNLOCK(fd,0,SEEK_SET,0);
    }
}
