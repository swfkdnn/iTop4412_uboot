#include "/home/sw/pp_self/1/iTop4412_uboot/Board_Drivers/sw_exynos4412_reg.h"
#include "stdio.h"
//void (*printf)(char *, ...) = 0x43e11434;
extern void uart_init(void);

extern void putc(unsigned char c);

int mmu_main(void)
{
    uart_init();
    printf("hello mmu~~~\n\r");
    unsigned long main_id, cpu_feature, c1_flags;
    __asm__ __volatile__(
          "mrc p15, 0, %0, c0, c0, 0\n"
          "mrc p15, 0, %1, c0, c1, 0\n"
          "mrc p15, 0, %2, c1, c0, 0\n"
          :"=r"(main_id), "=r"(cpu_feature), "=r"(c1_flags)
    );
    printf("MAIN ID:        0x%08x\n\r", main_id);
    printf("CPU FEATURE:    0x%08x\n\r", cpu_feature);
    printf("C1:             0x%08x\n\r", c1_flags);
    c1_flags = (1 << 3)|(1 << 11)|(1 << 28);
    __asm__ __volatile__(
          "mcr p15, 0, %0, c1, c0, 0\n"
          "mrc p15, 0, %0, c1, c0, 0\n"
          :"+r" (c1_flags)
    );
    printf("------------------------\n\r");
    printf("C1:             0x%08x\n\r", c1_flags);
    return 0;
}
