#define und
//#define swi

#include "/home/sw/pp_self/1/iTop4412_uboot/Board_Drivers/sw_exynos4412_reg.h"
#include "stdio.h"
void enable_mmu(unsigned long ttb);
void swi_init_ttb(unsigned long *tt_base);
void swi_mmap(unsigned long *tt_base, unsigned long va, unsigned long pa);
void swi_memset(char *buf, char ch, int size);
void swi_memcpy(char *dst, char *src, int size);
#ifdef und
void do_und(unsigned long regs[]);
#endif
#ifdef swi
void do_swi(unsigned long regs[]);
#endif

extern void uart_init(void);

extern void putc(unsigned char c);

//void (*printf)(char *,...) = 0x43e11434;


int swi_main(void)
{
  uart_init();

#ifdef und
  printf("hello und~~~\n\r");
#endif
#ifdef swi
  printf("hello swi~~~\n\r");
#endif

  unsigned long vector_base = 0xffff0000;
  unsigned long tt_base = 0x53000000;
#ifdef und
  unsigned long *pdo_und = 0x54000000;
#endif
#ifdef swi
  unsigned long *pdo_swi = 0x55000000;
#endif
  extern unsigned long vectors_start,vectors_end;

  swi_memset(tt_base,0x00,16*1024);
  swi_mmap(tt_base,vector_base,0x50000000);
  enable_mmu(tt_base);
  swi_memcpy(vector_base,vectors_start,0x100);
#ifdef und
  *pdo_und = do_und;
#endif
#ifdef swi
  *pdo_swi = do_swi;
#endif

  __asm__ __volatile__(
      "mov r0, r0\n"
#ifdef und
      ".word 0x77777777\n"  //未定义指令，swi,在译码阶段发生，lr = pc+4。其他异常lr = pc+8
      "mov r1, r1\n"
#endif
#ifdef swi
      "swi 0x7777\n"
      "mov r1, r1\n"
      "mov r2, r2\n"
#endif
  );
  printf("end of undefined.\n\r");
  return 0;
}


#ifdef und
void do_und(unsigned long regs[])
{
  printf("undefined instruction.\n\r");
}
#endif

#ifdef swi
/*
 * regs[0] = r0
 * regs[1] = r1
 * regs[12] = r12
 * regs[13] = lr
 * 寄存器环境传参
 */
void do_swi(unsigned long regs[])
{
  unsigned long *instr = regs[13] - 4;
  printf("swi: 0x%x\n\r", *instr & 0xffffff); //对lr-4地址取值，再取低24位，获得中断号0x7777
}
#endif

void enable_mmu(unsigned long ttb)
{
  unsigned long c1_flags;
  swi_init_ttb(ttb);
  c1_flags = 1|(1<<3)|(1<<11)|(1<<13)|(1<<28);
  __asm__ __volatile__(
      "mvn r0, #0 \n"

      "mcr p15, 0, r0, c3, c0, 0\n"
      "mcr p15, 0, %1, c2, c0, 0\n"  //configure ttb

      "mrc p15, 0, r0, c1, c0, 0\n"
      "orr %0, r0, %0\n"
      "mcr p15, 0, %0, c1, c0, 0\n"  //enable mmu
      :
      :"r"(c1_flags),"r"(ttb)
      :"r0"
  );

}

void swi_init_ttb(unsigned long *ttb_base)
{
    unsigned long va, pa;
    for(va = 0x00000000; va < 0x10000000; va += 0x100000){ //Others
        pa = va;
        ttb_base[ va >> 20] = (pa & 0xfff00000) | 2;
    }

    for(va = 0x10000000; va < 0x14000000; va += 0x100000){  //SFR
        pa = va;
        ttb_base[ va >> 20] = (pa & 0xfff00000) | 2;
    }

    for(va = 0x40000000; va < 0x80000000; va += 0x100000){ //DRAM   ??????????(512)
        pa = va;
        ttb_base[ va >> 20] = (pa & 0xfff00000) | 2;
    }
}

void swi_mmap(unsigned long *ttb_base, unsigned long va, unsigned long pa)
{
    ttb_base[ va >> 20] = (pa & 0xfff00000) | 2;

}

void swi_memset(char *buf, char ch, int size)
{
    int i;
    for(i=0;i<size;i++)
      buf[i] = ch;
}

void swi_memcpy(char *dst, char *src, int size)
{
    int i;
    for(i=0;i<size;i++)
      dst[i] = src[i];

}

//no volatile !!!!
__asm__ (
"vectors:\n"
       "b reset\n"
       "b und\n"
       "b swi\n"
       "b pre_abt\n"
       "b dat_abt\n"
       ".word 0\n"
       "b irq\n"
       "b fiq\n"

"reset:\n"
"und:\n"
       "mov sp, #0x54000000\n"
       "stmfd sp!, {r0-r12, lr}\n"

       "mov r0, sp\n"
       "mov r3, #0x54000000\n"
       "ldr r3, [r3]\n"
       "blx r3\n"                 //，blx并将处理器的工作状态有ARM 状态切换到Thumb 状态

       "mov sp, #0x54000000\n"
       "ldmea sp, {r0-r12, pc}^\n"

"swi:\n"
#ifdef swi
        "stmfd sp!, {r0-r12, lr}\n"  //栈从 高地址 向 低地址 存储
        "mov r0, sp\n"
        "mov r3, #0x55000000\n"   //传递参数，执行do_swi可以得到寄存器环境的值，r0,r1.r12等
        //"ldr r3, =0x55000000\n"
        //"ldr r3, [0x55000000]\n"
        "ldr r3, [r3]\n"
        //"ldr r3, =0x88\n"
        "blx r3\n"
        //"blx =0x55000000\n"

        "ldmea sp, {r0-r12, pc}^\n"
#endif
"pre_abt:\n"
"dat_abt:\n"
"fiq:\n"
"irq:\n"
        "mov sp, #0x57000000\n"
        "sub lr, lr, #4\n"
        "stmfd sp!, {r0-r12, lr}\n"

        "mov r0, sp\n"
        //"mov r3, #0x57000000\n"
        //"ldr r3, [r3]\n"
        //"bl r3\n"
        //"blx r3\n"
        "bl =0x57000000\n"

        "mov sp, #0x75000000\n"
        "ldmea sp, {r0-r12, pc}^\n"

"EOV:\n"
"vectors_start:\n"
        ".word vectors\n"
"vectors_end:\n"
        ".word EOV\n"
  );
