/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * date:2018/1/13
 */
#ifndef SHM_H
#define SHM_H
#include<sys/types.h>
#include<sys/shm.h>
#include<string>
#include<pthread.h>
/**
 * @brief:
 * 1.该类持有两个共享内存片段，第一个片段用于数据共享;第二个片段用于
 * 进程间同步，实际上共享的是一把跨进程的线程锁。该锁保护第一个片段的数据在
 * 多个进程间同步;
 * 2.同一个key实例化的所有对象共享同一块内存。所以如果想要在多个进程间共享
 * 内存必须保证key一致（即用相同的path和pro_id初始化对象）。
 * 3.需要手动释放系统的共享内存及相关数据结构。
 * @bug:该类无法在任意时间任意位置创建实例。在类实例化过程中，分配
 * 锁所占用的共享内存时无法保证多个进程间同步。即多个进程同时实例化占用同一块
 * 共享内存的对象时，可能会使某些进程实例化失败。故该类最好的使用方式是保证分
 * 享同一块共享内存的所有对象中的第一个对象实例化过程在无竞争的情况下进行（即
 * 提前创建一个对象）。之后可以在任意时间任意位置创建实例。其中只有在实例化第
 * 一个的对象时会创建共享内存。
 */

class Shm{
public:
    Shm(const char* arg_path="./",const int arg_id=15);
    ~Shm();

    void delete_shm();
    void get_shminfo();
    int process_lock();
    int process_unlock();
    friend void process_test();
private:
    bool attach_shm();//链接共享内存
    bool unattach_shm();//取消和共享内存之间的连接
    pthread_mutex_t process_share_mutex_init();//初始化进程锁

    std::string path;
    int pro_id;

    key_t data_key=-1;
    key_t mutex_key=-1;

    int shm_data_id=-1;
    int shm_mutex_id=-1;

    void* data_addr=nullptr;
    pthread_mutex_t* mutex_addr=nullptr;

    struct shmid_ds shmid_info;
    pthread_mutex_t shared_mutex_lock;
};

void process_test();
#endif // SHM_H
