#include<sys/types.h>
#include<sys/fcntl.h>
#include<sys/epoll.h>
#include<errno.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include"fifo.h"
void read_process()
{
    int fd=open ( "./_fifo",O_RDONLY|O_NONBLOCK ); //当以只读且非阻塞模式打开有名管道(FIFO)时，程序会阻塞在open函数调用，直到其它进程以写模式打开该FIFO
    std::cout<<"open after"<<std::endl;
    if ( fd<0 )
    {
        std::cout<<"open fifo fail"<<std::endl;
        perror ( "open" );
    }

    int ep_fd=epoll_create ( 2 );
    struct epoll_event ev,ev_arr[10];
    ev.events=EPOLLIN;
    ev.data.fd=fd;
    epoll_ctl ( ep_fd,EPOLL_CTL_ADD,fd,&ev );
    struct FifoInfo info_buffer;
    while ( 1 )
    {
        int nfds=epoll_wait ( ep_fd,ev_arr,10,-1 );
        if ( nfds<0 )
            std::cout<<"epoll wait fail!"<<std::endl;
        for ( int i=0; i<nfds; i++ )
        {
            if ( ev_arr[i].data.fd==fd )
            {
                ssize_t nr=read ( ev_arr[i].data.fd,&info_buffer,sizeof(info_buffer));
                //               std::cout<<"nr="<<nr<<std::endl;
                if ( nr<0 )
                {
                    std::cout<<"nr <0"<<std::endl;
                    if ( errno==EINTR )
                    {
                        std::cout<<"读操作被信号中断"<<std::endl;
                        continue;
                    }
                    if ( errno==EAGAIN )
                    {
                        std::cout<<"无数据可读"<<std::endl;
                        continue;
                    }
                }
                else if ( nr==0 )
                {
                    std::cout<<"对端关闭"<<std::endl;
                    goto CLOSE;
                }
                std::cout<<"flag="<<info_buffer.flag<<"     continuous="<<info_buffer.times.tv_sec<<std::endl;
            }
        }
    };

CLOSE:
    close ( fd );
    unlink ( "./_fifo" );
    return;
}


void write_process()
{
    int fd=open ("./_fifo",O_WRONLY|O_NONBLOCK);////当以只写且非阻塞模式打开有名管道(FIFO)时，程序会阻塞在open函数调用，直到其它进程以读模式打开该FIFO
    if ( fd<0 )
    {
        perror ( "open" );
    }
    for(int c=1;c<1000;c++)
    {
        std::cout<<"write "<<c<<std::endl;
        struct FifoInfo fifo_info;
        fifo_info.flag=1;
        fifo_info.times.tv_sec=c;
        ssize_t nw=write ( fd,&fifo_info,sizeof(fifo_info));
        if(nw==0)
        {
            perror("write");
        }
        if ( nw<0)
        {
            goto CLOSE;
            perror("nw <0");
            continue;
        }
    }
CLOSE:
    close(fd);
}
