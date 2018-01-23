#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <iostream>
#include <sys/epoll.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <vector>
#include "pipe.h"
#include "fifo.h"
#include "shm.h"
enum class ExecProcess{
    PIPE,FIFO,SOCK,SHARE_MEM
};

int main(int agrc,char* argv[] )
{
    if(argv[1]==nullptr)
    {
        std::cout<<"usage: process_name [0|1|2|3|4|5]"<<std::endl;
        return -1;
    }
    switch(atoi(argv[1]))
    {
    case (int)ExecProcess::PIPE:
    {
        Pipe my_pipe;
        break;
    }
    case (int)ExecProcess::FIFO:
    {
        pid_t pid=fork();
        if(pid<0)
        {
            return -1;
        }
        else if(pid>0)
        {
            //等待子进程结束再开始运行，而该进程的子进程会在开启之后马上结束
            int status;
            while(1)
            {
                if(waitpid(pid,&status,WNOHANG)!=0)
                {
                    break;
                }
            };


            int ret=mkfifo("./_fifo",S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH );
            if ( ret<0 )
            {
                std::cout<<"mkfifo fail"<<std::endl;
                perror ( "mkfifo" );
            }
            read_process();
        }
        else
        {
            pid_t pid2=fork();//再fork一次，避免僵尸进程
            if(pid2!=0)
            {
                exit(0);
            }
            else
            {
                sleep(1);//睡眠1秒保证管道读端已经被打开，因为如果没有进程为读而打开管道，则当以只写方式打开管道会失败，open返回-1且error设置为ENXIO
                write_process();
                exit(0);
            }
        }
        break;
    }
    case (int)ExecProcess::SOCK:
    {
        clockid_t clock_id;
        struct timespec time_buf;
        clock_gettime(clock_id,&time_buf);
        break;
    }
    case (int)ExecProcess::SHARE_MEM:
    {
        std::vector<pid_t>pid_vec(2);
        Shm share_memory;
        for(int i=0;i<2;i++)
        {
            pid_t pid=fork();
            if(pid<0)
                return -1;
            else if(pid==0)
            {
                process_test();
                exit(-1);
            }
            else
                pid_vec[i]=pid;
        }
        while(true)
        {
            if(waitpid(pid_vec[0],nullptr,WNOHANG)!=0 && waitpid(pid_vec[1],nullptr,WNOHANG))
                break;
        };

        share_memory.get_shminfo();
        share_memory.delete_shm();
        break;
    }
    default :
        break;
    }



    return 0;
}

