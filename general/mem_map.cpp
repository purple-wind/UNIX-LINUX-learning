/**
*   name:     share_mem.cpp
*   author:   purple-wind
*   email:
*   created:  2023-02-09 18:12:22
*   brief:    __attribute__((ifunc))属性用于支持开发者创建一个既有函数的多个实现，并通过开发者编写的函数注解在运行时来选择具体的某个实现
*             IFUNC，全称为 indirect function，是由 GNU toolchain 实现的一个特性，特性描述可以通过以下关键词来掌握：
*              1.可以为某个函数创建多个实现
*              2.在运行时使用解析器函数（resolver function）进行选择。
*              3.解析器函数由动态加载程序调用，选择实际的实现。（注：静态链接下同样也支持 IFUNC，但本文只分析动态链接下的处理）
*              4.选择完成后，函数实现就是固定的，在进程的生存期内不得更改。
*  本代码以共享内存为例子说明该属性
*/

#include<sys/mman.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<iostream>
#include<string.h>
#include<stdio.h>
#include<unistd.h>
#include<fstream>

using namespace std;
int get_config()
{
    ifstream ifs("./123.txt", std::ios::in);
    int type = 0;
    ifs >> type;
    return type;
}

//内存映射到一个文件，多个进程映射到同一个文件，则相当于在多个进程间共享数据，注意锁保护数据同步
void test_file_mmap()
{
    cout<<__FUNCTION__<<endl;
    size_t size = 1024*1024*1024;
    int fd = open("/home/xxx.txt", O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
    if(-1 == fd)
        perror("open");
    ftruncate(fd, size);
    void *ptr = mmap(0, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if(ptr == MAP_FAILED)
        cout<<__FUNCTION__<<": mmap fail"<<endl;
    *(size_t*)ptr = size;
    close(fd);
    cout<<"mem size="<<*(size_t *)ptr<<endl;
    memcpy(ptr, "hello world", 11);
    munmap(ptr, size);
}

//匿名内存无法在进程间共享使用
void test_anon_mem()
{
    cout<<__FUNCTION__<<endl;
    size_t size = 5*1024*1024*1024U;
    void *ptr = mmap(0, size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    if(ptr == MAP_FAILED)
        perror("mmap");
    memcpy(ptr, "hello world", 11);
    munmap(ptr, size);
}

//此时获取到的内存是从大叶内存中分配
void test_hupage_mem()
{
    cout<<__FUNCTION__<<endl;
    size_t size = 5*1024*1024*1024U;
    void *ptr = mmap(0, size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS|MAP_HUGETLB, -1, 0);
    if(ptr == MAP_FAILED)
        perror("mmap");
    memcpy(ptr, "hello world", 11);
    munmap(ptr, size);
}

//该属性只能用于c语言,c++不支持，如果想在c++中使用，需要extern "C"{}声明
void test_mem() __attribute__((ifunc ("test_mem_resolver")));
#ifdef __cplusplus
extern "C"{
#endif
/*该函数会在进程全局初始化时执行，在main函数之前运行,且只会执行一次去决定要使用的函数，在进程的整个生命期内都使用该选中函数*/
void (*test_mem_resolver(void))()
{
    int type = get_config();
    printf("test_mem_resolver type=%d\n", type);
    if(type == 1)
        return test_file_mmap;
    else if(type == 2)
        return test_anon_mem;
    else if (type == 3)
        return test_hupage_mem;
    else
        return test_file_mmap;
}
#ifdef __cplusplus
}
#endif

void interface()
{
    for(int i = 0; i < 3; i++)
        test_mem();
}


int main(int argc , char * argv[])
{
    cout<<"main"<<endl;
    interface();
    return 0;
}
