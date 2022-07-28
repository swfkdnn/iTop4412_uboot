/*
 *    XadcAIN0
 *    Analog Input Range: 0 ~ 1.8V
 *    ADC数据的读取通常由2种方法：
 *    [1]中断模式=Interrupt mode；[2]轮询模式=round robin mode，浪费cpu资源。
 */

#define round_robin_mode     /* [2]轮询模式 */
//#define Interrupt_mode       /* [1]中断模式 */

#include "/home/sw/pp_self/1/iTop4412_uboot/Board_Drivers/sw_exynos4412_reg.h"
#include "stdio.h"
#include "key.h"

extern void uart_init(void);
extern void putc(unsigned char c);

void mydelay_ms(long time)
{
    printf("delay %ld ms \n\r",time);
    int i, j;
    long a = time;
//    while(time--)  //error  ??? 跑飞
    while(a--)
    {
//        printf("delay %ld ms \n\r",a);
      if(a % 10000000 == 0)
            printf("a/1000 = %d s \n\r", a/1000);
        for (i = 0; i < 5; i++)
            for (j = 0; j < 514; j++);
    }
}

/* round robin mode */
#ifdef round_robin_mode

unsigned char table[10] = {'0','1','2','3','4','5','6','7','8','9'};

void adc_init(int temp)
{
    printf("rADCCON = 0x%x \n\r", rADCCON);
    rADCCON = (1<<16|1<<14|99<<6|1<<1);
    printf("() = 0x%x \n\r", (1<<16|1<<14|99<<6|1<<1));
    printf("rADCCON = 0x%x \n\r", rADCCON);
    rADCMUX = 0x0;
    temp = rADCDAT & 0xfff;
}

/*
 *  裸机代码，不同于LINUX 应用层， 一定加循环控制
 */
int sw_adc_work(void)
{
    unsigned char bit4,bit3,bit2,bit1;
    unsigned int temp = 0;
    adc_init(temp);
//    puts("开始转换\n");
    printf("开始转换\n\r");
//    mydelay_ms(30000000);
//    while(1)
//    {
        while(!(rADCCON & 0x8000));
        temp = rADCDAT & 0xfff;
        printf("U = %d -----\n\r",temp);
//        temp = 1.8 * 1000 * temp/0xfff;
//        bit4 = temp /1000;
//        putc(table[bit4]);
//        bit3 = (temp % 1000)/100;
//        putc(table[bit3]);
//        bit2 = ((temp % 1000)%100)/10;
//        putc(table[bit2]);
//        bit1 = ((temp % 1000)%100)%10;
//        putc(table[bit1]);
//        //puts("mV");
//        putc('\n');
//        printf("U =%ld%ld%ld%ld mV \n\r",table[bit4],table[bit3],table[bit2],table[bit1]);
//        printf("U =%ld mV \n\r",table[bit4]);
        mydelay_ms(30000000);
//    }
   return 0;
}
#endif


/* Interrupt_mode  */
#ifdef Interrupt_mode
/* 
 *  SPI Port No       10           (0-127)    Software Generated Interrupts (SGIs[15:0], ID[15:0])--专属ICDSGIR
 *  ID                42           (0-159)    Private Peripheral Interrupts (PPIs[15:0], ID[31:16]
 *  Int_I_Combiner    IntG10_3                Shared Peripheral Interrupts (SPIs[127:0], ID[159:32])
 *  Interrupt Source  ADC
 *  Source Block      General ADC
 */

void do_irq(void)
{
       int id,cpu_id;
       int adc_data;
       id = rICCIAR_CPU0 & 0x3ff;
       cpu_id = (rICCIAR_CPU0 >> 10) & 0x7;
       printf("in irq id = %d \n\r cpu_id = %d \n\r",id,cpu_id);
       switch(id)
       {
         case 42:
              adc_data = rADCDAT & 0xfff;
              printf("adc_data = %d \n\r",adc_data);
              rCLRINTADC = 0;
       //    IECR2 = IECR2 | (1 << 19);               打开的话只能读取一次，
              //42/32
              //清除中断状态位外设级[1]，EXT_INT41_PEND |= 0x1 << 1;本例没有
              rICDICPR1  = rICDICPR1  | (1 << 10);
              //【清GIC中断标志位类似于 ICDISER】清除中断状态位GIC级[2]
              break;
       }
       rICCEOIR_CPU0 = rICCEOIR_CPU0 & (~0x3ff) | id;  
       //The interrupt ID 清除中断状态位 CPU0级[3]
}

void adc_init(void)
{    //12bit   使能分频       分频值                 手动
    printf("rADCCON = 0x%x \n\r", rADCCON);
    rADCCON = (1<<16|1<<14|99<<6|1<<1);
    printf("() = 0x%x \n\r", (1<<16|1<<14|99<<6|1<<1));
    printf("rADCCON = 0x%x \n\r", rADCCON);
    rADCMUX = 0x0;
}

void adcint_init(void)
{
    //arm 中断  --> GIC中断 --> EXT——INT 外部中断   依次打开
    rIESR2  = rIESR2 | (1 << 19);
    rICDDCR = 1;    //a、使能分配器   启用外设中断通过GIC转发给CPU

    //42/32
    rICDISER1 = rICDISER1 | (1 << 10);
    //b、使能相应中断到分配器 32位寄存器 每个中断占1位 160个中断只用5个寄存器 
    //42号ADC中断

    rICDIPTR10 = rICDIPTR10 &(~(0xff << 16)) | (0x1 << 16);
    //8位寄存器最多支持8核 exynos四核cpu    c、42号ADC中断发给cpu0 0b00000001

    //rICDIPR10 = 0;   /*0 is Highest priority 每个中断占8位 中断ID=0开始 42号中断ADC rICDIPR10[23:16]*/
    rICCPMR_CPU0  = 255;//设置中断屏蔽优先级 低于此优先级的不发信号

    rICCICR_CPU0 = 1;  //全局使能开关  d、GIC总开关向cpu0发信号
    printf("rIESR2 = 0x%x \n\r", rIESR2);
    printf("rICDDCR = 0x%x \n\r", rICDDCR);
    printf("rICDIPR10 = 0x%x \n\r", rICDIPR10);
    printf("rICDIPTR10 = 0x%x \n\r", rICDIPTR10);
    printf("rICDISER1 = 0x%x \n\r", rICDISER1);
    printf("rICCPMR_CPU0 = 0x%x \n\r", rICCPMR_CPU0);
    printf("rICCICR_CPU0 = 0x%x \n\r", rICCICR_CPU0);
    printf("adcint_init done --------\n\r");
}

int sw_adc_work(void)
{
       adc_init();
       adcint_init();
       while(1)
       {
              rADCCON = rADCCON | 1;
              printf("rADCCON = 0x%x \n\r", rADCCON);
              mydelay_ms(30000000);
       }
   return 0;
}
#endif

int adc_main(void)
{
    uart_init();
#ifdef round_robin_mode     /* [2]轮询模式 */
    printf("in ADC...round_robin_mode.\n\r");
#endif
#ifdef Interrupt_mode       /* [1]中断模式 */
    printf("in ADC...Interrupt_mode .\n\r");
#endif
//    mydelay_ms(4000);
    int r = sw_adc_work();
    printf("r = %d\n",r);
    return 0;
}






