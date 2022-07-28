/*
*	Filename   : start.S
*	Author     : Kilento 
*   Description: To Control LED on iTop-4412
*	Date       : 2021-8-15
*/

/*

LED2  KP_COL0（NetLable） XGNSS_GPIO_0/GPL2_0  	高电平导通（NPN三极管）
LED3  VDD50_EN(NetLable)  Xmmc1CMD/GPK1_1  	高电平导通（NPN三极管）

查阅数据手册《SEC_Exynos4412_Users Manual_Ver.1.00.00》"6.General Purpose Input/Output (GPIO) Control"部分
可以得知控制 GPL2_0 和 GPK1_1 引脚的一些寄存器物理地址
GPL2_0：

GPL2CON(用于控制GPIO的模式)：
Base Address: 0x1100_0000
Address = Base Address + 0x0100, Reset Value = 0x0000_0000
将其配置成输出模式：GPL2CON[3:0]=0x1

GPL2DAT(输出模式时，用于控制GPIO的输出电平)：
	Base Address: 0x1100_0000
	Address = Base Address + 0x0104, Reset Value = 0x00
控制GPL2_0的输出电平：GPL2[0]=0|1

其余寄存器不需要用到

GPK1CON(用于控制GPIO的模式)：
Base Address: 0x1100_0000
Address = Base Address + 0x0060, Reset Value = 0x0000_0000
将其配置成输出模式：GPKCON[7:4]=0x1
GPK1DAT(输出模式时，用于控制GPIO的输出电平)：
	Base Address: 0x1100_0000
	Address = Base Address + 0x0064, Reset Value = 0x00
控制GPK_1的输出电平：GPLK[1]=0|1
	
*/

#include "s3c4412_gpio.h"
#include "s3c4412_sdram.h"

#define POW_MNG_UNIT_BASE 0x10020000 
#define PS_HOLD_CONTROL (POW_MNG_UNIT_BASE + 0x330C)
#define WATCHDOG		  0x10060000

/* 占位符 4字共16字节，预留用来加 bl1 校验头，填充什么数据无所谓 */
/* 
.word 0x0
.word 0x0
.word 0x0
.word 0x0
*/
/*
.text
.global _start
*/
_start:

	/* 将 PS_HOLD_CONTRO 置为输出模式且使能。如果不设置在设备冷启动时程序运行一遍后就停止，重新按复位才会重新运行程序。详细原因后面再介绍 */
	ldr r0, =PS_HOLD_CONTROL
	ldr r1, [r0]
	#orr r1, r1, #0x300
	ldr r1, =0x0300
	str r1, [r0]


/* 将 GPX0PUD Disable上下拉模式，如果不设置发现程序每隔几秒重新运行。这里原理还没理解，知道后再补充 */
	ldr r0, =GPX0PUD
	ldr r1, =0x0
	str r1, [r0]

	/* 关闭看门狗 */
	ldr r0, =WATCHDOG
	ldr r1, =0x0
	str r1, [r0]
	
	ldr sp, =0x02050000
	
	/* 打开或者关闭 iCache */
	mrc p15, 0, r0, c1, c0, 0  /* 将 CP15 寄存器 C1 的值读到 r0 中 */
	//bic r0, r0, #(1<<12)	   /* bit12 置0 关闭 iCache */
	orr r0, r0, #(1<<12)	   /* bit12 置1 开启 iCache */
	mcr p15, 0, r0, c1, c0, 0  /* 将 r0 的值写到 CP15 寄存器 C1 中 */
	/* 时钟初始化 */
	bl lowlevel_init
	/* 初始化sdram */
	bl mem_init

	adr r0, _start				/* 短加载运行时地址 */
	ldr r1, =_start				/* 长加载链接时地址 */
	ldr r2, =bss_start			/* 链接脚本中定义的 bss_start 地址 */
	cmp r0, r1
	beq run_on_newaddr      	/* 如果相等说明不需要重定位，所以跳过copy_loop, 直接到 run_on_newaddr，否则执行 copy_loop */

/* 拷贝需要重定位的代码段和数据段 */
copy_loop:
	ldr r3, [r0], #4 			/* 从r0 拷贝4字节内存到 r3 寄存器 */
	str r3, [r1], #4			/* 只需拷贝代码段和数据段，.bss段无需拷贝，因为bss段中的数据都是0 */
	cmp r1, r2					/* 比较长加载链接时地址 和 bss_start 链接时地址 */
	bne copy_loop				
	
/* 清零重定位后的bss段 */
clean_bss:
	ldr r0, =bss_start
	ldr r1, =bss_end
	cmp r0, r1				/* 如果 r0 = r1，说明 .bss 为空，直接跳转 */
	beq run_on_newaddr
	mov r2, #0				/* 初始化 [r2] = 0 */
clear_loop:
	str r2, [r0], #4        /* 将 r2 中的值存入 r0 所指向的地址，然后 r0 += 4 */
	cmp r0, r1				
	bne clear_loop
	
run_on_newaddr:	
/*	ldr pc, = main 	*//* 使用长加载进行地址跳转，不能再使用 bl led_blink 进行跳转 */
	ldr pc, = swi_main
b .



