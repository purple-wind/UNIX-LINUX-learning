#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
    long ID1;

    ID1 = getpid();
    printf("pid=%ld\n", ID1);

    long ID2;
    asm volatile(
              "int $0x80\n\t"
              : "=a" (ID2)     /* eax 亦是返回值所在，因此要与ID1 对应 */
              : "0" (20)         /* 20 置入 eax */
        );

    printf("pid=%ld\n", ID2);

    return 0;
}
