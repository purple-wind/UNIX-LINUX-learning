/*inotify是一个监视文件系统内文件状态变化的机制,
 可监测文件的新建、删除、修改等,当一个文件夹内的文件
 发生变化则事件被通知；
 简单示例:
 */
 #include<sys/inotify.h>
 #include<syslog.h>
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

 int main(void)
 {
     test_inotify();
 }
