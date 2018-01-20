#ifndef PIPE_H
#define PIPE_H
#include<unistd.h>
#include<time.h>
//pope只能用于父子进程之间通信，无父子进程之间的关系的进程无法使用pipe
class Pipe
{
public:
    Pipe();
<<<<<<< HEAD
   size_t ReadPipe(unsigned char* buffer, size_t len);
   size_t WritePipe(unsigned char *buffer, size_t len);
    private:
=======
private:
>>>>>>> origin/master
    int pipe_fd[2];
};

#endif // PIPE_H
