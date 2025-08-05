#include <stddef.h>
#include <stdint.h>

static void out(uint16_t port, uint32_t value)
{
    asm("out %0,%1" : /* empty */ : "a"(value), "Nd"(port) : "memory");
}

void
    __attribute__((noreturn))
    __attribute__((section(".start")))
    _start(void)
{

    uint32_t array[5];
    uint32_t counter = 0;

    for (;;)
    {
        for (int i = 0; i < 5; i++)
        {
            array[i] = counter++;
        }
        out(0x10, (uint32_t)(uintptr_t)array);
    }

    *(long *)0x400 = 42;
    for (;;)
        asm("hlt" : /* empty */ : "a"(42) : "memory");
}
