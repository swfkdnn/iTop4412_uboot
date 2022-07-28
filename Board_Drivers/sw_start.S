/*
 *Filename   : start.S
 *Author     : sw 
 *Description: test
 *Date       : 2022-07-22
 */

#include "s3c4412_gpio.h"
#include "s3c4412_sdram.h"
#include "/home/sw/pp_self/1/iTop4412_uboot/Board_Drivers/sw_exynos4412_reg.h"


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
.text
.global _start
_start:
        b       reset
        ldr     pc,_undefined_instruction
        ldr     pc,_software_interrupt
        ldr     pc,_prefetch_abort
        ldr     pc,_data_abort
        ldr     pc,_not_used
        ldr     pc,_irq
        ldr     pc,_fiq

_undefined_instruction: .word  _undefined_instruction
_software_interrupt:    .word  _software_interrupt
_prefetch_abort:        .word  _prefetch_abort
_data_abort:            .word  _data_abort
_not_used:              .word  _not_used
_irq:                   .word  irq_handler
_fiq:                   .word  _fiq


reset:
    ldr r0,=0x40000000
    mcr p15,0,r0,c12,c0,0       @ Vector Base Address Register















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



init_stack:
    ldr     r0,stacktop         /*get stack top pointer*/

/********svc mode stack********/
    mov     sp,r0
    sub     r0,#128*4          /*512 byte  for irq mode of stack*/
/****irq mode stack**/
    msr     cpsr,#0xd2
    mov     sp,r0
    sub     r0,#128*4          /*512 byte  for irq mode of stack*/
/***fiq mode stack***/
    msr     cpsr,#0xd1
    mov     sp,r0
    sub     r0,#0
/***abort mode stack***/
    msr     cpsr,#0xd7
    mov     sp,r0
    sub     r0,#0
/***undefine mode stack***/
    msr     cpsr,#0xdb
    mov     sp,r0
    sub     r0,#0
/*** sys mode and usr mode stack ***/
    msr     cpsr,#0x10
    mov     sp,r0             /*1024 byte  for user mode of stack*/


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
	ldr pc, = main 	/* 使用长加载进行地址跳转，不能再使用 bl led_blink 进行跳转 */
	
b .

.align  4

/****  swi_interrupt handler  ****/


/****  irq_handler  ****/
irq_handler:

    sub  lr,lr,#4
    stmfd sp!,{r0-r12,lr}
    .weak do_irq
    bl  do_irq
    ldmfd sp!,{r0-r12,pc}

stacktop:    .word      stack+4*512
.data

stack:   .space  4*512




