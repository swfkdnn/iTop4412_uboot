#include <config.h>
#include <version.h>
#include <s5pc210.h>
#include "smdk4x12_val.h" //mj

#define MEM_DLLl_ON

wait_mux_state:
	ldr r1, [r0, r2]
	cmp r1, r3
	bne wait_mux_state
	mov pc, lr

wait_pll_lock:
	ldr r1, [r0, r2]
	tst r1, #(1<<29)
	beq wait_pll_lock
	mov pc, lr

wait_phy_state:
	ldr r1, [r0, #DMC_PHYSTATUS]
	tst r1, #(1<<2)
	beq wait_phy_state
	mov pc, lr
/* end add */

_TEXT_BASE:
	.word	TEXT_BASE

	.globl iTop4412_uboot_lowlevel_init
iTop4412_uboot_lowlevel_init:
/*
 *   uboot刚开始并不知道dram是否初始化了，所以使⽤iram
 */
	ldr	sp, =0x02060000
	push	{lr}

/* check reset status  */
	ldr r0, =(INF_REG_BASE + INF_REG1_OFFSET)               /* INFORM1:0x10020800+4 */
	ldr r1, [r0]

/* AFTR wakeup reset */
	ldr	r2, =S5P_CHECK_DIDLE                                /* 0xBAD00000 */
	cmp	r1, r2
	beq	exit_wakeup

/* Sleep wakeup reset */
	ldr	r2, =S5P_CHECK_SLEEP                                /* 0x00000BAD */
	cmp	r1, r2
	beq	wakeup_reset

/* PS-Hold high */
	ldr r0, =0x1002330c                                     /*power hold*/
	ldr r1, [r0]
	orr r1, r1, #0x300
	str r1, [r0]

/*  Disables Pull-up/Pull-down  */
	ldr r0, =0x11000c08                                      /* no reset */
	ldr r1, =0x0
	str r1, [r0]

/* Clear  MASK_WDT_RESET_REQUEST  */
	ldr r0, =0x1002040c
	ldr r1, =0x00
	str r1, [r0]

#define check_mem
#ifdef check_mem                   /* 检测是否需要重载 */
	ldr	r0, =0xff000fff              /* 1111 1111 0000 0000 0000 1111 1111 1111 */
	bic	r1, pc, r0                   /* r0 <- current base addr of code */
	ldr	r2, _TEXT_BASE               /* r1 <- original base addr in ram */
	bic	r2, r2, r0                   /* r0 <- current base addr of code */
	cmp r1, r2                       /* compare r0, r1 */
/* 如果 r1 和 r2 相等就说明是在DDR运行,就要跳过时钟初始化和初始化DDR。 */
/* 如果 r1 和 r2 不相等就说明是在 SRAM 运行,就要进行 时钟初始化 和 初始化DDR。*/
	beq 1f                           /* r0 == r1 then skip sdram init */
#endif

ldr	r0, =CHIP_ID_BASE
ldr	r1, [r0]                       /*  PRO_ID */
lsr	r1, r1, #8                     /* r1 >> 8 */
and	r1, r1, #3                     /* Package Information */
cmp	r1, #2                         /* Exynos 4412:0x2 */
bne v310_1

	bl system_clock_init_scp
	bl mem_ctrl_asm_init_ddr3
	bl tzpc_init
	b 1f

v310_1:                            /* do */
	bl	system_clock_init_scp
	bl	mem_ctrl_asm_init_ddr3

























1:
#ifdef CONFIG_EVT1___
/* store DMC density information in DRAM */
/* mem_ctrl_asm_init returns dmc_density in r6 */
	ldr	r0, =CFG_UBOOT_BASE
	sub	r0, r0, #4
	str	r6, [r0]
#endif


#if 0                     /* test for (wifi)mt6620 turn off GPC1(0) */
/*wenpin.cui: headphone and sw uart switch init*/
	ldr r0, =0x11000C44                             /*GPX2DAT*/
	ldr r1, [r0]
	and r1, r1, #0x4
	cmp r1, #0x4                                    /* uart */
	beq out
	ldr r0, =0x11400084                             /* GPC1(0)  GPC1DAT   */
	ldr r1, [r0]                                    /* read GPC1DAT status*/
	orr r1, r1, #0x1                                /* GPC1(0) output high  */
	str r1, [r0]
	ldr r0, =0x11400080                             /* GPC1(0)  GPC1CON   */
	ldr r1, [r0]
	and r1, r1, #0xfffffff0
	orr r1, r1, #0x1                                /* GPC1(0) output  */
	str r1, [r0]
#endif





out:
/* for UART */
	bl uart_asm_init

#if defined(CONFIG_ONENAND)
	bl onenandcon_init
#endif

#if defined(CONFIG_NAND)
/* simple init for NAND */
	bl nand_asm_init
#endif

/* Print 'K' */
	ldr	r0, =ELFIN_UART_CONSOLE_BASE
	ldr	r1, =0x4b4b4b4b
	str	r1, [r0, #UTXH_OFFSET]











/* 2010.08.27 by icarus : for temporary 3D clock fix */
	ldr	r1, =0x1
	ldr	r2, =0x1003C22C
	str	r1, [r2]
	ldr	r1, =0x2
	ldr	r2, =0x1003C52C
	str	r1, [r2]

/* 2010.10.17 by icarus : for temporary MFC clock fix */
	ldr	r1, =0x3
	ldr	r2, =0x1003C528
	str	r1, [r2]
	pop	{pc}

wakeup_reset:
	ldr	r0, =CHIP_ID_BASE
	ldr	r1, [r0]
	lsr	r1, r1, #8
	and	r1, r1, #3
	cmp	r1, #2
	bne	wake_v310


/* check C2C_CTRL enable bit */
	ldr r3, =SYSC_PMU_BASE
	ldr r1, [r3, #C2C_CTRL_OFFSET]
	and r1, r1, #1
	cmp r1, #0
	bne skip_dmc

/* init system clock */
/* add by cym 20130218 */
	bl	mem_ctrl_asm_init_ddr3
/* end add */

skip_dmc:
/* add by cym 20130218 */
	bl	system_clock_init_scp
	bl	tzpc_init
	b	exit_wakeup

wake_v310:
/* add by cym 20130218 */
	bl system_clock_init_scp
	bl mem_ctrl_asm_init_ddr3
	bl tzpc_init

exit_wakeup:
/*Load return address and jump to kernel*/
	ldr	r0, =(INF_REG_BASE+INF_REG0_OFFSET)
	ldr	r1, [r0]                               /* r1 = physical address of s5pc110_cpu_resume function*/
	mov	pc, r1                                 /*Jump to kernel */
	nop
	nop

system_clock_init_scp:
	push	{lr}
	ldr	r0, =ELFIN_CLOCK_BASE                  /* 0x1003_0000 */

@ CMU_CPU MUX / DIV
	ldr	r1, =0x0
	ldr	r2, =CLK_SRC_CPU_OFFSET
	str	r1, [r0, r2]
	ldr r2, =CLK_MUX_STAT_CPU_OFFSET
	ldr r3, =0x01110001
	bl wait_mux_state
	ldr	r1, =CLK_DIV_DMC0_VAL
	ldr	r2, =CLK_DIV_DMC0_OFFSET
	str	r1, [r0, r2]
	ldr	r1, =CLK_DIV_DMC1_VAL
	ldr	r2, =CLK_DIV_DMC1_OFFSET
	str	r1, [r0, r2]

@ CMU_TOP MUX / DIV
	ldr	r1, =0x0
	ldr	r2, =CLK_SRC_TOP0_OFFSET
	str	r1, [r0, r2]
	ldr r2, =CLK_MUX_STAT_TOP_OFFSET
	ldr r3, =0x11111111
	bl wait_mux_state
	ldr	r1, =0x0
	ldr	r2, =CLK_SRC_TOP1_OFFSET
	str	r1, [r0, r2]
	ldr r2, =CLK_MUX_STAT_TOP1_OFFSET
	ldr r3, =0x01111110
	bl wait_mux_state
	ldr	r1, =CLK_DIV_TOP_VAL
	ldr	r2, =CLK_DIV_TOP_OFFSET
	str	r1, [r0, r2]

@ CMU_LEFTBUS MUX / DIV
	ldr	r1, =0x10
	ldr	r2, =CLK_SRC_LEFTBUS_OFFSET
	str	r1, [r0, r2]
	ldr r2, =CLK_MUX_STAT_LEFTBUS_OFFSET
	ldr r3, =0x00000021
	bl wait_mux_state
	ldr	r1, =CLK_DIV_LEFRBUS_VAL
	ldr	r2, =CLK_DIV_LEFTBUS_OFFSET
	str	r1, [r0, r2]

@ CMU_RIGHTBUS MUX / DIV
	ldr	r1, =0x10
	ldr	r2, =CLK_SRC_RIGHTBUS_OFFSET
	str	r1, [r0, r2]

	ldr r2, =CLK_MUX_STAT_RIGHTBUS_OFFSET
	ldr r3, =0x00000021
	bl wait_mux_state

	ldr	r1, =CLK_DIV_RIGHTBUS_VAL
	ldr	r2, =CLK_DIV_RIGHTBUS_OFFSET
	str	r1, [r0, r2]

@ Set PLL locktime
	ldr	r1, =APLL_LOCK_VAL
	ldr	r2, =APLL_LOCK_OFFSET
	str	r1, [r0, r2]

	ldr	r1, =MPLL_LOCK_VAL
	ldr	r2, =MPLL_LOCK_OFFSET
	str	r1, [r0, r2]

	ldr	r1, =EPLL_LOCK_VAL
	ldr	r2, =EPLL_LOCK_OFFSET
	str	r1, [r0, r2]

	ldr	r1, =VPLL_LOCK_VAL
	ldr	r2, =VPLL_LOCK_OFFSET
	str	r1, [r0, r2]

	ldr	r1, =CLK_DIV_CPU0_VAL
	ldr	r2, =CLK_DIV_CPU0_OFFSET
	str	r1, [r0, r2]
	ldr	r1, =CLK_DIV_CPU1_VAL
	ldr	r2, =CLK_DIV_CPU1_OFFSET
	str	r1, [r0, r2]

@ Set APLL
	ldr	r1, =APLL_CON1_VAL
	ldr	r2, =APLL_CON1_OFFSET
	str	r1, [r0, r2]
	ldr	r1, =APLL_CON0_VAL
	ldr	r2, =APLL_CON0_OFFSET
	str	r1, [r0, r2]

/* check MPLL and if MPLL is not 400 Mhz skip MPLL resetting for C2C operation */
	ldr	r2, =MPLL_CON0_OFFSET
	ldr r1, [r0, r2]
	ldr r3, =0xA0640301
	cmp r1, r3
	bne skip_mpll

@ Set MPLL
	ldr	r1, =MPLL_CON1_VAL
	ldr	r2, =MPLL_CON1_OFFSET
	str	r1, [r0, r2]
	ldr	r1, =MPLL_CON0_VAL
	ldr	r2, =MPLL_CON0_OFFSET
	str	r1, [r0, r2]

skip_mpll:
@ Set EPLL
	ldr     r1, =EPLL_CON2_VAL
	ldr     r2, =EPLL_CON2_OFFSET
	str     r1, [r0, r2]
	ldr     r1, =EPLL_CON1_VAL
	ldr     r2, =EPLL_CON1_OFFSET
	str     r1, [r0, r2]
	ldr     r1, =EPLL_CON0_VAL
	ldr     r2, =EPLL_CON0_OFFSET
	str     r1, [r0, r2]

@ Set VPLL
	ldr     r1, =VPLL_CON2_VAL
	ldr     r2, =VPLL_CON2_OFFSET
	str     r1, [r0, r2]
	ldr     r1, =VPLL_CON1_VAL
	ldr     r2, =VPLL_CON1_OFFSET
	str     r1, [r0, r2]
	ldr     r1, =VPLL_CON0_VAL
	ldr     r2, =VPLL_CON0_OFFSET
	str     r1, [r0, r2]

	ldr r2, =APLL_CON0_OFFSET
	bl wait_pll_lock
	ldr r2, =MPLL_CON0_OFFSET
	bl wait_pll_lock
	ldr r2, =EPLL_CON0_OFFSET
	bl wait_pll_lock
	ldr r2, =VPLL_CON0_OFFSET
	bl wait_pll_lock

	ldr	r1, =0x01000001
	ldr	r2, =CLK_SRC_CPU_OFFSET
	str	r1, [r0, r2]

	ldr r2, =CLK_MUX_STAT_CPU_OFFSET
	ldr r3, =0x02110002
	bl wait_mux_state

	ldr	r1, =0x00011000
	ldr	r2, =CLK_SRC_DMC_OFFSET
	str	r1, [r0, r2]

	ldr r2, =CLK_MUX_STAT_DMC_OFFSET
	ldr r3, =0x11102111
	bl wait_mux_state

	ldr	r1, =0x00000110
	ldr	r2, =CLK_SRC_TOP0_OFFSET
	str	r1, [r0, r2]

	ldr r2, =CLK_MUX_STAT_TOP_OFFSET
	ldr r3, =0x11111221
	bl wait_mux_state

/* skip MUX_ACLK_200_SUB_SEL, MUX_ACLK_400_MCUISP_SUB_SEL setting for CMU_SYSCLK_ISP function */
	ldr	r1, =0x00011000
	ldr	r2, =CLK_SRC_TOP1_OFFSET
	str	r1, [r0, r2]

	ldr r2, =CLK_MUX_STAT_TOP1_OFFSET
	ldr r3, =0x01122110
	bl wait_mux_state

	ldr	r0, =CHIP_ID_BASE
	ldr	r1, [r0]
	lsr	r1, r1, #8
	and	r1, r1, #3
	cmp	r1, #2
	bne	v310_2

/* check C2C_CTRL enable bit */
	ldr r3, =S5PV310_POWER_BASE
	ldr r1, [r3, #C2C_CTRL_OFFSET]
	and r1, r1, #1
	cmp r1, #0
	bne v310_2

@ ConControl
#ifdef MEM_DLLl_ON
	ldr	r0, =APB_DMC_0_BASE

	ldr	r1, =0x7F10100A
	ldr	r2, =DMC_PHYCONTROL0
	str	r1, [r0, r2]

	ldr	r1, =0xE0000084
	ldr	r2, =DMC_PHYCONTROL1
	str	r1, [r0, r2]

	ldr	r1, =0x7F10100B
	ldr	r2, =DMC_PHYCONTROL0
	str	r1, [r0, r2]

	bl wait_phy_state

	ldr	r1, =0x0000008C
	ldr	r2, =DMC_PHYCONTROL1
	str	r1, [r0, r2]
	ldr	r1, =0x00000084
	ldr	r2, =DMC_PHYCONTROL1
	str	r1, [r0, r2]

	bl wait_phy_state

	ldr	r0, =APB_DMC_1_BASE

	ldr	r1, =0x7F10100A
	ldr	r2, =DMC_PHYCONTROL0
	str	r1, [r0, r2]

	ldr	r1, =0xE0000084
	ldr	r2, =DMC_PHYCONTROL1
	str	r1, [r0, r2]

	ldr	r1, =0x7F10100B
	ldr	r2, =DMC_PHYCONTROL0
	str	r1, [r0, r2]

	bl wait_phy_state

	ldr	r1, =0x0000008C
	ldr	r2, =DMC_PHYCONTROL1
	str	r1, [r0, r2]
	ldr	r1, =0x00000084
	ldr	r2, =DMC_PHYCONTROL1
	str	r1, [r0, r2]

	bl wait_phy_state
#endif

	ldr	r0, =APB_DMC_0_BASE
	ldr	r1, =0x0FFF30FA
	ldr	r2, =DMC_CONCONTROL
	str	r1, [r0, r2]

	ldr	r0, =APB_DMC_1_BASE
	ldr	r1, =0x0FFF30FA
	ldr	r2, =DMC_CONCONTROL
	str	r1, [r0, r2]

	ldr	r0, =APB_DMC_0_BASE
	ldr	r1, =0x00202533
	ldr	r2, =DMC_MEMCONTROL
	str	r1, [r0, r2]

	ldr	r0, =APB_DMC_1_BASE
	ldr	r1, =0x00202533
	ldr	r2, =DMC_MEMCONTROL
	str	r1, [r0, r2]

v310_2:                                 /* do */
	pop	{pc}









/*
Check clock until stable.
Check devider change state 
*/
wait_div_state1:
	ldr	r1, =(ELFIN_CLOCK_POWER_BASE + CLK_DIV_STAT1_OFFSET)
        ldr     r2, [r1]
	tst	r2, #(0x1<<15)
	bne	wait_div_state1
	mov	pc, lr
	
/* Check source change state */
wait_mux_state1:
	ldr	r1, =(ELFIN_CLOCK_POWER_BASE + CLK_MUX_STAT1_OFFSET)
        ldr     r2, [r1]
	tst	r2, #(0x1<<31)
	bne	wait_mux_state1
	mov	pc, lr
	
/* Check source change state */
wait_mux_state0:
	ldr	r1, =(ELFIN_CLOCK_POWER_BASE + CLK_MUX_STAT0_OFFSET)
        ldr     r2, [r1]
	tst	r2, #(0x4)
	bne	wait_mux_state0
	mov	pc, lr

/*
 * uart_asm_init: Initialize UART in asm mode, 115200bps fixed.
 * void uart_asm_init(void)
 */
.globl uart_asm_init
uart_asm_init:

/* set GPIO to enable UART */
	@ GPIO setting for UART for UART0/1
	ldr	r0, =0x11400000
	ldr	r1, =0x22222222
	str   	r1, [r0]
	ldr	r0, =0x11400020
	ldr	r1, =0x222222
	str	r1, [r0]

	ldr	r0, =CMU_BASE	@0x1003_0000
	ldr	r1, =CLK_SRC_PERIL0_VAL		@0x666666
	ldr	r2, =CLK_SRC_PERIL0_OFFSET
	str	r1, [r0, r2]
	ldr	r1, =CLK_DIV_PERIL0_VAL		@0x777777
	ldr	r2, =CLK_DIV_PERIL0_OFFSET
	str	r1, [r0, r2]

	ldr	r0, =ELFIN_UART_CONSOLE_BASE		@0x13800000
	ldr	r1, =0x111
	str	r1, [r0, #UFCON_OFFSET]

	mov	r1, #0x3
	str	r1, [r0, #ULCON_OFFSET]

	ldr	r1, =0x3c5
	str	r1, [r0, #UCON_OFFSET]

	ldr	r1, =UART_UBRDIV_VAL	@0x2B /*  UDIV(43) = ((82,500,000 / 16 / 115200) - 1) */
	str	r1, [r0, #UBRDIV_OFFSET]

	ldr	r1, =UART_UDIVSLOT_VAL	@0xC /* UFRACVAL(12) = ((((82,500,000 / 16 / 115200) - 1) - 43) * 16) */
	str	r1, [r0, #UDIVSLOT_OFFSET]

	ldr	r1, =0x4f4f4f4f
	str	r1, [r0, #UTXH_OFFSET]		@'O'

	mov	pc, lr

/* OneNAND Interface Init */
onenandcon_init:
wait_orwb:
	@; Read ONENAND_IF_STATUS
	ldr	r0, =CFG_ONENANDXL_BASE		@; 0x0C600000
	ldr	r1, [r0, #0x100]		@; ONENAND_IF_STATUS_OFFSET(0x100)
	bic	r1, r1, #0xFFFFFFFE
	cmp	r1, #0x0

	@; ORWB != 0x0
	bne	wait_orwb

	@; write new configuration to onenand system configuration1 register
	ldr	r1, =0xF006			@; Sync.
	ldr	r2, =(CFG_ONENAND_BASE+0x1E442)	@; 0x1E442(REG_SYS_CONF1)
	strh	r1, [r2]

	@; read one dummy halfword
	ldrh	r1, [r2]
	ldrh	r1, [r2]

	@; write new configuration to ONENAND_IF_CTRL
	ldr	r0, =CFG_ONENANDXL_BASE		@; 0x0C600000
	@;ldr	r1, =0x2F006			@; ONENAND_IF_CTRL_REG_VAL (GCE off)
	ldr	r1, =0x402F006			@; ONENAND_IF_CTRL_REG_VAL (GCE on)
	str	r1, [r0, #0x100]		@; ONENAND_IF_STATUS_OFFSET(0x100)

	mov	pc, lr

/* Nand Interface Init for SMDKC100 */
.globl nand_asm_init
nand_asm_init:

/* Setting GPIO for NAND */
/* This setting is NAND initialze code at booting time in iROM. */
	ldr	r0, =0x11000000
	ldr     r1, [r0, #MP01CON_OFFSET]
	bic     r1, r1, #(0xf<<8)
	orr     r1, r1, #(0x3<<8)
	str     r1, [r0, #MP01CON_OFFSET]

@	ldr	r1, [r0, #MP01CON_OFFSET]
@	bic	r1, r1, #(0xf<<8)
@ orr r1, r1, #((0x3<<8)|(0x2<<16)|(0x2<<20))
@ orr r1, r1, #0x220300
@ ldr r2, =0x220300
@ bic r1, r1, r2
@	str	r1, [r0, #MP01CON_OFFSET]

	ldr r1, [r0, #MP01PUD_OFFSET]
	bic r1, r1, #(0x3<<4)
	str r1, [r0, #MP01PUD_OFFSET]

	ldr r1, [r0, #MP03CON_OFFSET]
	bic r1, r1, #0xFFFFFF

	ldr r2, =0x222
	orr r1, r1, r2
	str r1, [r0, #MP03CON_OFFSET]

	ldr r0, =ELFIN_NAND_BASE
	ldr r1, [r0, #NFCONF_OFFSET]

	ldr r2, =0x7772
	bic	r1, r1, r2

	ldr	r2, =NFCONF_VAL
	orr	r1, r1, r2
	str	r1, [r0, #NFCONF_OFFSET]
	ldr	r1, [r0, #NFCONT_OFFSET]
	ldr r2, =0x6
	bic	r1, r1, r2

	ldr	r2, =NFCONT_VAL

	orr	r1, r1, r2
	str	r1, [r0, #NFCONT_OFFSET]
	ldr     r1, [r0, #MP01CON_OFFSET]
	bic     r1, r1, #(0xf<<8)
	orr     r1, r1, #(0x3<<8)
	str     r1, [r0, #MP01CON_OFFSET]
	mov	pc, lr

/* Setting TZPC[TrustZone Protection Controller] */
tzpc_init:
	ldr	r0, =ELFIN_TZPC0_BASE
	mov	r1, #0x0
	str	r1, [r0]
	mov	r1, #0xff
	str	r1, [r0, #TZPC_DECPROT0SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT1SET_OFFSET]
	@@@;;str	  r1, [r0, #TZPC_DECPROT2SET_OFFSET] 	
	str	r1, [r0, #TZPC_DECPROT3SET_OFFSET]

	ldr 	r0, =ELFIN_TZPC1_BASE
	str	r1, [r0, #TZPC_DECPROT0SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT1SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT2SET_OFFSET] 	
	str	r1, [r0, #TZPC_DECPROT3SET_OFFSET]

	ldr	r0, =ELFIN_TZPC2_BASE
	str	r1, [r0, #TZPC_DECPROT0SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT1SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT2SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT3SET_OFFSET] 

	ldr	r0, =ELFIN_TZPC3_BASE
	str	r1, [r0, #TZPC_DECPROT0SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT1SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT2SET_OFFSET] 	
	str	r1, [r0, #TZPC_DECPROT3SET_OFFSET]

	ldr	r0, =ELFIN_TZPC4_BASE
	str	r1, [r0, #TZPC_DECPROT0SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT1SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT2SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT3SET_OFFSET]

	ldr	r0, =ELFIN_TZPC5_BASE
	str	r1, [r0, #TZPC_DECPROT0SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT1SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT2SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT3SET_OFFSET]

	mov	pc, lr


#ifdef CONFIG_ENABLE_MMU

/*
 * MMU Table for SMDKC210
 * 0x0000_0000 -- 0x1FFF_FFFF => A:0x0000_0000 -- 0x1FFF_FFFF
 * 0x2000_0000 -- 0x3FFF_FFFF => Not Allowed
 * 0x4000_0000 -- 0x5FFF_FFFF => A:0x4000_0000 -- 0x5FFF_FFFF
 * 0x6000_0000 -- 0xBFFF_FFFF => Not Allowed
 * 0xC000_0000 -- 0xDFFF_FFFF => A:0x4000_0000 -- 0X5FFF_FFFF
 * 0xE000_0000 -- 0xFFFF_FFFF => Not Allowed
 */

/* form a first-level section entry */
.macro FL_SECTION_ENTRY base,ap,d,c,b
	.word (\base << 20) | (\ap << 10) | \
	      (\d << 5) | (1<<4) | (\c << 3) | (\b << 2) | (1<<1)
.endm

.section .mmudata, "a"
.align 14
/* the following alignment creates the mmu table at address 0x4000. */
.globl mmu_table
mmu_table:
	.set __base,0

/* Access for iRAM  */
	.rept 0x200
	FL_SECTION_ENTRY __base,3,0,0,0
	.set __base,__base+1
	.endr

/* Not Allowed  */
	.rept 0x400 - 0x200
	.word 0x00000000
	.endr

	.set __base,0x400
/* 512MB for SDRAM with cacheable */
	.rept 0x600 - 0x400
	FL_SECTION_ENTRY __base,3,0,1,1
	.set __base,__base+1
	.endr

/* access is not allowed. */
	.rept 0xc00 - 0x600
	.word 0x00000000
	.endr

	.set __base,0x400
/* 512MB for SDRAM with cacheable */
	.rept 0xE00 - 0xC00
	FL_SECTION_ENTRY __base,3,0,1,1
	.set __base,__base+1
	.endr

/* access is not allowed. */
	.rept 0x1000 - 0xE00
	.word 0x00000000
	.endr

#endif

