/*inotify是一个监视文件系统内文件状态变化的机制,
 可监测文件的新建、删除、修改等,当一个文件夹内的文件
 发生变化则事件被通知；
 简单示例:
 */
#include<sys/inotify.h>
#include<syslog.h>
#include<unistd.h>
#include<stdio.h>
#define EVENT_BUF_LEN 256
#define EVENT_SIZE 8
 int test_inotify(void)
 {
     int length;
     int inotify_fd;
     int wd;
     char buffer[EVENT_BUF_LEN];

     inotify_fd =inotify_init();
     if (inotify_fd < 0) {
         syslog(LOG_ERR,"inotify init fail!");
     }

     /*adding the"./" directory into watch list. Here, the suggestion
     *is to validate the existence of the directory before adding into
     * monitoring list.
     */
     wd = inotify_add_watch(inotify_fd,"./",IN_CLOSE);


     /*read to determine the event change happens on "./" directory.
     *Actually this read blocks until the change event occurs
     */
     while(1)
     {
         int i = 0;
         length = read(inotify_fd,buffer,EVENT_BUF_LEN);
         if (length < 0){
             syslog(LOG_ERR,"inotify read length fail!");
         }

         /*actually read return the list of change events happens.
         *Here, read the change event one by one and process it accordingly.
         */
         while (i < length)
         {
             struct inotify_event *event = (struct inotify_event *) &buffer[i];
             if (event->len) {
                 if ((event->mask & IN_CLOSE)) {
                     printf("file was closed\n");
                     printf("file_name =%s",event->name);
                 }
             }
             i += EVENT_SIZE + event->len;
         };
     };

     inotify_rm_watch(inotify_fd, wd);
     close(inotify_fd);
     return 0;
 }

//监控单个文件
//此处有个问题：常用的编辑器（例如vim或者ide工具）修改文件时，其实是把原始文件内容加载到内存，然后删除原始文件，
//再用内存中的内容写到跟原始文件同名的文件中。这个过程中文件的inode number发生了变化，对应到inotify中就会触发
//IN_MOVE_SELF事件，然后inotify无法继续监控该文件。因此在监控单个文件的情况下，为了继续监控该文件，需要再次
//用inotify_add_watch添加对该文件的监控。但是这里存在一个bug就是如果IN_MOVE_SELF后立即结束循环再次调用inotify_add_watch
//会提示找不到该文件。ubuntu下实测需要等待差不多1s左右的时间才可以inotify_add_watch成功，目前不清楚该问题的具体原因。

int test_inotify_file(void)
{
    int length;
    int inotify_fd;
    int wd;
    char buffer[EVENT_BUF_LEN];

    inotify_fd =inotify_init();
    if (inotify_fd < 0) {
        std::cout<<"inotify init fail!"<<std::endl;
    }
    std::cout<<"inotify fd ="<<inotify_fd<<std::endl;

    while(true)
    {
        wd = inotify_add_watch(inotify_fd, "/root/test.txt", IN_ALL_EVENTS);
        if(wd < 0)
        {
            perror("error reason");
            sleep(1);//如果不等待1s则下面的inotify_add_watch会失败
            wd = inotify_add_watch(inotify_fd, "/root/test.txt", IN_ALL_EVENTS);
        }
        std::cout<<"wd="<<wd<<std::endl;
        while(true)
        {
            memset(buffer, 0x00, EVENT_BUF_LEN);
            std::cout<<"----------------------------"<<std::endl;
            std::cout<<"before"<<std::endl;
            length = read(inotify_fd, buffer, EVENT_BUF_LEN);
            std::cout<<"read length="<<length<<std::endl;
            struct inotify_event *event = (struct inotify_event *)buffer;
            std::cout<<"wd="<<event->wd<<std::endl;
            std::cout<<"mask="<<event->mask<<std::endl;
            std::cout<<"cookie="<<event->cookie<<std::endl;
            std::cout<<"len="<<event->len<<std::endl;
            std::cout<<"name="<<event->name<<std::endl;
            if(event->mask & IN_MOVE_SELF)
            {
                std::cout<<"IN_MOVE_SELF"<<std::endl;
                inotify_rm_watch(inotify_fd, wd);
                break;
            }
        };

        std::cout<<"after while"<<std::endl;
    }

    inotify_rm_watch(inotify_fd, wd);
    close(inotify_fd);
    return 0;
}

 int main(void)
 {
     test_inotify();
      test_inotify_file();
 }
