#ifndef BYTE_RANGE_LOCKING_H
#define BYTE_RANGE_LOCKING_H
void father_process();
void child_process_rclock();
void wait_child(pid_t child_pid);
#endif // BYTE_RANGE_LOCKING_H

