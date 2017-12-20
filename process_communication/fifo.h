#ifndef FIFO_H
#define FIFO_H

#include<unistd.h>
#include<string>

//定义一个fifo之间传输数据的结构
struct FifoInfo{
    bool flag;
    struct timeval times;
};




/*
 *FIFO(有名管道)的几点说明：
 * 1.如果管道未设置O_NONBLOCK，则以读方式打开管道时，管道会阻塞，直到有进程为写而打开。以写方式打开管道时，管道也会阻塞，知道有进程为读而打开;
 * 2.如果管道设置了O_NONBLOCK,则以只读方式打开管道时，open不会阻塞，open立即返回且成功打开。以只写方式打来管道时open也立即返回，但是open返回-1,此时的
 * error被设置为ENXIO
 * 3.当write一个尚无进程为读而打开的FIFO时，进程会收到SIGPIPE(管道破裂)信号;
 * 4.当PIPE的最后一个写进程关闭PIPE时，则将为该FIFO的读进程产生一个文件结束标志;
 * 5.当read一个写端已经关闭的管道时，在所有数据都被读取后，read返回0;
 *
 *
 */

void write_process();
void read_process();

#endif // FIFO_H
