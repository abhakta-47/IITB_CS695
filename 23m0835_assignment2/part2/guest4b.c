#include <stddef.h>
#include <stdint.h>

static void out(uint16_t port, int32_t value)
{
    asm("out %0,%1" : /* empty */ : "a"(value), "Nd"(port) : "memory");
}

static uint64_t rdtsc()
{
    unsigned int lo, hi;
    __asm__ volatile("rdtsc" : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
}
static uint32_t get_random_number()
{
    return rdtsc() % 11;
}

void
    __attribute__((noreturn))
    __attribute__((section(".start")))
    _start(void)
{
    *(long *)0x400 = 42;

    uint32_t buffer[20];
    int32_t prod_p = -1, cons_p = -1;

    out(0x10, (uint32_t)(uintptr_t)buffer);
    out(0x11, (uint32_t)(uintptr_t)&prod_p);
    out(0x12, (uint32_t)(uintptr_t)&cons_p);

    for (;;)
    {

        uint32_t n = get_random_number();
        if (prod_p != -1 && cons_p == -1)
            cons_p = 0;
        for (uint32_t i = 0; i < n; i++)
        {
            if (cons_p == prod_p)
                break;
            cons_p = (cons_p + 1) % 20;
        }
        out(0x15, cons_p);
    }
    asm("hlt" : /* empty */ : "a"(42) : "memory");
}
