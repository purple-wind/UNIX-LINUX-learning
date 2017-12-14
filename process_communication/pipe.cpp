#include "pipe.h"
#include <iostream>
Pipe::Pipe()
{
 if(pipe(pipe_fd)<0)
 {
   return;
 }
 std::cout<<"fd[0]="<<pipe_fd[0]<<std::endl;
 std::cout<<"fd[1]="<<pipe_fd[1]<<std::endl;
 return;
}

size_t Pipe::ReadPipe(unsigned char* buffer,size_t len)
{
 return read(pipe_fd[0],buffer,len);
}

size_t Pipe::WritePipe(unsigned char* buffer, size_t len)
{
  return write(pipe_fd[1],buffer,len);
}
