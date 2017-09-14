#linux-3.13.0 gcc 5.3.0

#include<stdio.h>
#include<stdint.h>
int main()
{  
//移位操作与操作数的类型有关,当移位 的位数大于类型的位数时结果是未定义的，
//比如：把一个32位数左移33位结果未定义，有可能为1；编译器会报出waring：left shift count >= width of type [-Wshift-count-overflow]

  uint32_t e=1;
  printf("e=%x\n",(e<<33));
  return 0;
}
