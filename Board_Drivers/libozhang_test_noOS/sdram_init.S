#include "s3c4412_sdram.h"

#define MIU_2BIT_21_7_INTERLEAVED
#define MEM_DLL
#define DRAM_CLK_400

#define DMC_IVCONTROL	0xF0
wait_phy_state:
	ldr r1, [r0, #DMC_PHYSTATUS]
	tst r1, #(1<<2)
	beq wait_phy_state
	mov pc, lr

dmc_delay:
	push {lr}
1:	subs r2, r2, #1
	bne 1b
	pop {pc}
	
.global mem_init

mem_init:
	push {lr}

	ldr	r0, =APB_DMC_0_BASE 

	ldr	r1, =0x0
	str	r1, [r0, #DMC_PHYCONTROL2]

	ldr	r1, =0x0
	str	r1, [r0, #0x24]

	ldr	r1, =0xE3855503
	str	r1, [r0, #DMC_PHYZQCONTROL]

	ldr	r1, =0x71101008				
	str	r1, [r0, #DMC_PHYCONTROL0]

	ldr	r1, =0x7110100A				
	str	r1, [r0, #DMC_PHYCONTROL0]

	ldr	r1, =0x20000086				
	str	r1, [r0, #DMC_PHYCONTROL1]

	ldr	r1, =0x71101008				
	str	r1, [r0, #DMC_PHYCONTROL0]

	ldr	r1, =0x2000008E				
	str	r1, [r0, #DMC_PHYCONTROL1]

	ldr	r1, =0x20000086				
	str	r1, [r0, #DMC_PHYCONTROL1]

	ldr	r1, =0x2000008E				
	str	r1, [r0, #DMC_PHYCONTROL1]

	ldr	r1, =0x20000086				
	str	r1, [r0, #DMC_PHYCONTROL1]

	ldr	r1, =0x0FFF30CA
	str	r1, [r0, #DMC_CONCONTROL]

	ldr	r1, =0x00302600				
	str	r1, [r0, #DMC_MEMCONTROL]

	/* 4Gb * 4 */
	/*
	ldr r1, =0x40801323
	str	r1, [r0, #DMC_MEMCONFIG0]
	*/

	/* 2Gb * 8 */

	ldr r1, =0x40801333
	str	r1, [r0, #DMC_MEMCONFIG0]
	
	ldr	r1, =(0x80000000 | CONFIG_IV_SIZE)
	str	r1, [r0, #DMC_IVCONTROL]

	ldr	r1, =0x64000000			
	str	r1, [r0, #DMC_PRECHCONFIG]

	ldr	r1, =0x9C4000FF			
	str	r1, [r0, #DMC_PHYCONTROL0]

	ldr	r1, =0x000000BB
	str	r1, [r0, #DMC_TIMINGAREF] @TimingAref

	ldr	r1, =0x7846654F/*0x4046654F*/
	str	r1, [r0, #DMC_TIMINGROW] @TimingRow
	ldr	r1, =0x46400506    				
	str	r1, [r0, #DMC_TIMINGDATA] @TimingData
	ldr	r1, =0x52000A3C    				
	str	r1, [r0, #DMC_TIMINGPOWER] @TimingPower

	/* minimum wait time is 100 nano seconds */
	/* 0x64: wait 250 nano seconds at ARMCLK 1.5 Ghz */
	mov	r2, #0x64
	bl dmc_delay

	ldr	r1, =0x07000000       				
	str	r1, [r0, #DMC_DIRECTCMD] 

	/* minimum wait time is 200 micro seconds */
	/* 0x19000: wait 250 micro seconds at ARMCLK 1.5 Ghz */
	mov	r2, #0x19000
	bl dmc_delay

	ldr	r1, =0x00020000       				
	str	r1, [r0, #DMC_DIRECTCMD]

	/* minimum wait time is 20 micro seconds */
	/* 0x2700: wait 25 micro seconds at ARMCLK 1.5 Ghz */
	mov	r2, #0x2700
	bl dmc_delay

	ldr	r1, =0x00030000       				
	str	r1, [r0, #DMC_DIRECTCMD] 

	/* minimum wait time is 1 micro seconds */
	/* 0x3f0: wait 2.5 micro seconds at ARMCLK 1.5 Ghz */
	mov	r2, #0x3f0
	bl dmc_delay

	ldr	r1, =0x00010000
	str	r1, [r0, #DMC_DIRECTCMD] 
	ldr	r1, =0x00000100       				
	str	r1, [r0, #DMC_DIRECTCMD] 

	mov	r2, #0x3f0
	bl dmc_delay

	ldr	r1, =0x00000420       				
	str	r1, [r0, #DMC_DIRECTCMD]

	mov	r2, #0x3f0
	bl dmc_delay

	ldr	r1, =0x0A000000
	str	r1, [r0, #DMC_DIRECTCMD]

	mov	r2, #0x3f0
	bl dmc_delay

/*****************************************************************/
/*DREX1***********************************************************/
/*****************************************************************/
	ldr	r0, =APB_DMC_1_BASE 

	ldr	r1, =0x0
	str	r1, [r0, #DMC_PHYCONTROL2]

	ldr	r1, =0x0
	str	r1, [r0, #0x24]

	ldr	r1, =0xE3855503
	str	r1, [r0, #DMC_PHYZQCONTROL]

	ldr	r1, =0x71101008				
	str	r1, [r0, #DMC_PHYCONTROL0]

	ldr	r1, =0x7110100A				
	str	r1, [r0, #DMC_PHYCONTROL0]

	ldr	r1, =0x20000086				
	str	r1, [r0, #DMC_PHYCONTROL1]

	ldr	r1, =0x71101008				
	str	r1, [r0, #DMC_PHYCONTROL0]

	ldr	r1, =0x2000008E				
	str	r1, [r0, #DMC_PHYCONTROL1]

	ldr	r1, =0x20000086				
	str	r1, [r0, #DMC_PHYCONTROL1]

	ldr	r1, =0x2000008E				
	str	r1, [r0, #DMC_PHYCONTROL1]

	ldr	r1, =0x20000086				
	str	r1, [r0, #DMC_PHYCONTROL1]

	ldr	r1, =0x0FFF30CA
	str	r1, [r0, #DMC_CONCONTROL]

	ldr	r1, =0x00302600				
	str	r1, [r0, #DMC_MEMCONTROL]

	/* 4Gb * 4 */
	/*
	ldr r1, =0x40801323
	str	r1, [r0, #DMC_MEMCONFIG0]
	*/

	/* 2Gb * 8 */
	ldr r1, =0x40801333
	str	r1, [r0, #DMC_MEMCONFIG0]

	ldr	r1, =(0x80000000 | CONFIG_IV_SIZE)
	str	r1, [r0, #DMC_IVCONTROL]

	ldr	r1, =0x64000000			
	str	r1, [r0, #DMC_PRECHCONFIG]

	ldr	r1, =0x9C4000FF			
	str	r1, [r0, #DMC_PHYCONTROL0]

	ldr	r1, =0x000000BB
	str	r1, [r0, #DMC_TIMINGAREF] @TimingAref

	ldr	r1, =0x7846654F/*0x4046654F*/
	str	r1, [r0, #DMC_TIMINGROW] @TimingRow
	ldr	r1, =0x46400506    				
	str	r1, [r0, #DMC_TIMINGDATA] @TimingData
	ldr	r1, =0x52000A3C    				
	str	r1, [r0, #DMC_TIMINGPOWER] @TimingPower

	/* minimum wait time is 100 nano seconds */
	/* 0x64: wait 250 nano seconds at ARMCLK 1.5 Ghz */
	mov	r2, #0x64
	bl dmc_delay

	ldr	r1, =0x07000000       				
	str	r1, [r0, #DMC_DIRECTCMD] 

	/* minimum wait time is 200 micro seconds */
	/* 0x19000: wait 250 micro seconds at ARMCLK 1.5 Ghz */
	mov	r2, #0x19000
	bl dmc_delay

	ldr	r1, =0x00020000       				
	str	r1, [r0, #DMC_DIRECTCMD]

	/* minimum wait time is 20 micro seconds */
	/* 0x2700: wait 25 micro seconds at ARMCLK 1.5 Ghz */
	mov	r2, #0x2700
	bl dmc_delay

	ldr	r1, =0x00030000       				
	str	r1, [r0, #DMC_DIRECTCMD] 

	/* minimum wait time is 1 micro seconds */
	/* 0x3f0: wait 2.5 micro seconds at ARMCLK 1.5 Ghz */
	mov	r2, #0x3f0
	bl dmc_delay

	ldr	r1, =0x00010000
	str	r1, [r0, #DMC_DIRECTCMD] 
	ldr	r1, =0x00000100       				
	str	r1, [r0, #DMC_DIRECTCMD] 

	mov	r2, #0x3f0
	bl dmc_delay

	ldr	r1, =0x00000420       				
	str	r1, [r0, #DMC_DIRECTCMD]

	mov	r2, #0x3f0
	bl dmc_delay

	ldr	r1, =0x0A000000
	str	r1, [r0, #DMC_DIRECTCMD]

	mov	r2, #0x3f0
	bl dmc_delay


	ldr	r0, =APB_DMC_0_BASE

	ldr	r1, =0x7110100A
	ldr	r2, =DMC_PHYCONTROL0
	str	r1, [r0, r2]

	ldr	r1, =0x20000086
	ldr	r2, =DMC_PHYCONTROL1
	str	r1, [r0, r2]

	ldr	r1, =0x7110100B
	ldr	r2, =DMC_PHYCONTROL0
	str	r1, [r0, r2]

	bl wait_phy_state

	ldr	r1, =0x2000008E
	ldr	r2, =DMC_PHYCONTROL1
	str	r1, [r0, r2]
	ldr	r1, =0x20000086
	ldr	r2, =DMC_PHYCONTROL1
	str	r1, [r0, r2]

	bl wait_phy_state

	ldr	r0, =APB_DMC_1_BASE

	ldr	r1, =0x7110100A
	ldr	r2, =DMC_PHYCONTROL0
	str	r1, [r0, r2]

	ldr	r1, =0x20000086
	ldr	r2, =DMC_PHYCONTROL1
	str	r1, [r0, r2]

	ldr	r1, =0x7110100B
	ldr	r2, =DMC_PHYCONTROL0
	str	r1, [r0, r2]

	bl wait_phy_state

	ldr	r1, =0x2000008E
	ldr	r2, =DMC_PHYCONTROL1
	str	r1, [r0, r2]
	ldr	r1, =0x20000086
	ldr	r2, =DMC_PHYCONTROL1
	str	r1, [r0, r2]

	bl wait_phy_state

	ldr	r0, =APB_DMC_0_BASE
	ldr	r2, =DMC_CONCONTROL
	ldr	r1, [r0, r2]
	orr r1, r1, #(1 << 5)
	str r1, [r0, r2]

	ldr	r0, =APB_DMC_1_BASE
	ldr	r2, =DMC_CONCONTROL
	ldr	r1, [r0, r2]
	orr r1, r1, #(1 << 5)
	str r1, [r0, r2]

	ldr	r0, =APB_DMC_0_BASE
	ldr	r2, =DMC_MEMCONTROL
	ldr	r1, [r0, r2]
	orr	r1, r1, #((1 << 4) | (1 << 1) | (1 << 0))
	str	r1, [r0, r2]

	ldr	r0, =APB_DMC_1_BASE
	ldr	r2, =DMC_MEMCONTROL
	ldr	r1, [r0, r2]
	orr	r1, r1, #((1 << 4) | (1 << 1) | (1 << 0))
	str	r1, [r0, r2]

	pop {pc}