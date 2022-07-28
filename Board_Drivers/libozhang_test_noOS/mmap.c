#include "/home/sw/pp_self/1/iTop4412_uboot/Board_Drivers/sw_exynos4412_reg.h"
#include "stdio.h"


//void (*printf)(char *, ...) = 0x43e11433;

void init_ttb(unsigned long *ttb_base);
void mmap(unsigned long *ttb_base, unsigned long va, unsigned long pa);
void mmemset(char *buf, char ch, int size);

extern void uart_init(void);

extern void putc(unsigned char c);

void mmap_main(void)
{
    uart_init();
    printf("mmap~~~~\n\r");
    unsigned long c1_flags, ttb = 0x5f000000;
    volatile int *p = 0x52345678;

    *p = 0x99999999;

    printf("p_0 = 0x%08x\n\r", p);
    printf("*p_0 = 0x%08x\n\r", *p);

    init_ttb(ttb);

    mmap(ttb,0x12345678, 0x52345678);

    c1_flags = 1 | (1 << 3) | (1 << 11) | (1 << 28);

    __asm__ __volatile__(
        "mvn r0, #0 \n"
        "mcr p15, 0, r0, c3, c0, 0\n"

//        "mov r0, #0 \n"
//        "mcr p15, 0, r0, c7, c7, 0\n" //invalidate I&D cache
//        "mcr p15, 0, r0, c8, c7, 0\n" //invalifate I&D TLB


        "mcr p15, 0, %1, c2, c0, 0\n"  //congfigure ttb

        "mrc p15, 0, r0, c1, c0, 0\n"
        "orr %0, r0, %0\n"
        "mcr p15, 0, %0, c1, c0, 0\n"  //enable mmu
        :
        : "r"(c1_flags),"r"(ttb)
        : "r0"
    );
    p = 0x12345678;
    printf("p = 0x%08x\n\r", p);
    printf("*p = 0x%08x\n\r", *p);
}

void init_ttb(unsigned long *ttb_base)
{
    unsigned long va, pa;
    printf("ttb_base = 0x%08x\n\r", ttb_base);
    mmemset(ttb_base, 0x00, 16*1024);

    for(va = 0x00000000; va < 0x10000000; va += 0x100000){   //Others
        pa = va;
        ttb_base[va >> 20] = (pa & 0xfff00000) | 2;
    }

    for(va = 0x10000000; va < 0x14000000; va += 0x100000){   //SFR
        pa = va;
        ttb_base[va >> 20] = (pa & 0xfff00000) | 2;
//        printf("ttb_base[%d] = 0x%08x\n\r",(va >> 20),ttb_base[ va >> 20]);
    }

    for(va = 0x40000000; va < 0x80000000; va += 0x100000){   //DRAM
        pa = va;
        ttb_base[va >> 20] = (pa & 0xfff00000) | 2;
    }
}

void mmap(unsigned long *ttb_base, unsigned long va , unsigned long pa)//单条映射va(虚拟)->pa(物理)
{
    ttb_base[va >> 20] = (pa & 0xfff00000) | 2;
    printf("ttb_base[%d] = 0x%08x\n\r",(int)(va >> 20),ttb_base[ va >> 20]);
}

void mmemset(char *buf, char ch, int size)
{
    int i;
    for(i = 0; i < size; i++)
      buf[i] = ch;
}











