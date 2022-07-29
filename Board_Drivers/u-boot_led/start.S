#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define GPIOBASE 0x11000000

#define GPK1CON (GPIOBASE + 0x0060)
#define GPK1DAT (GPIOBASE + 0x0064)
#define GPK1PUD (GPIOBASE + 0x0068)
#define GPK1DRV (GPIOBASE + 0x006C)

#define GPL2CON (GPIOBASE + 0x0100)
#define GPL2DAT (GPIOBASE + 0x0104)
#define GPL2PUD (GPIOBASE + 0x0108)
#define GPL2DRV (GPIOBASE + 0x010C)

#define GPX0CON (GPIOBASE + 0x0C00)
#define GPX0DAT (GPIOBASE + 0x0C04)
#define GPX0PUD (GPIOBASE + 0x0C08)
#define GPX0DRV (GPIOBASE + 0x0C0C)

#define POW_MNG_UNIT_BASE 0x10020000 
#define PS_HOLD_CONTROL (POW_MNG_UNIT_BASE + 0x330C)
.word 0x0
.word 0x0
.word 0x0
.word 0x0

_start:
	ldr r0, =PS_HOLD_CONTROL                    /*power hold*/
	ldr r1, [r0]
	ldr r1, =0x0300
	str r1, [r0]

	ldr r0, =GPX0PUD                            /*no reset*/
	ldr r1, =0x0
	str r1, [r0]

	ldr r0, =GPL2CON
	mov r1, #(0x01 << 0)
	str r1, [r0]

	ldr r0, =GPK1CON
	mov r1, #(0x01 << 4)
	str r1, [r0]

	ldr r0, =GPK1DAT
	/*mov r1, #0x00        LED3 灭*/
	mov r1, #0x2         /*LED3 亮*/
	str r1, [r0]

	ldr r0, =GPL2DAT
	mov r1, #0x00        /*LED2 灭*/
	/*mov r1, #0x1         LED2 亮*/
	str r1, [r0]

	mov pc, lr
	b .
	#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
