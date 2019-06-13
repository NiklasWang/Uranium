#include <stdio.h>

int main()
{
    void *stackbase = 0;
    void *ptr = 0;
    printf("&stackbase = %p, &ptr = %p\n", &stackbase, &ptr);

    char array[2048] = { 0 };
    for (unsigned int i = 0; i < sizeof(array); i++) {
        array[i] = i % 26 + 'A';
    }
    printf("&array[2048] = %p\n", array);

    __asm__ __volatile__(
      "movq %%rsp, %0"
      :"=r"(ptr)
      );
    printf("rsp = %p\n", ptr);

    __asm__ (
      "movq %%rbp, %0"
      :"=r"(ptr)
    );
    printf("rbp = %p\n", ptr);

    void *stackbase2 = 0;
    __asm__ (
      "movq %%fs:8, %0"
      :"=r"(stackbase2)
    );
    printf("fs:0x8 = %p\n", stackbase2);

    return 0;
}