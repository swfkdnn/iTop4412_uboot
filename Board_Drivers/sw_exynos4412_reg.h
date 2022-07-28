/*
 *  sw
 *  2022-07-18
 *

 *  reference:  《SEC_Exynos4412_Users Manual_Ver.0.30.00_Preliminary》
 *
 *
 */


#ifndef SW_EXYNOS4412_REGS_H
#define SW_EXYNOS4412_REGS_H


/* Chip ID */

/* GPIO */

#define GPIO_BASE               0x11400000

#define GPD0CON                 (GPIO_BASE + 0x00A0)
#define GPD0DAT                 (GPIO_BASE + 0x00A4)
#define GPD0PUD                 (GPIO_BASE + 0x00A8)
#define GPD0DRV                 (GPIO_BASE + 0x00AC)
#define GPD0CONPDN              (GPIO_BASE + 0x00B0)
#define GPD0PUDPDN              (GPIO_BASE + 0x00B4)

#define GPK1CON                 (GPIO_BASE + 0x0060)
#define GPK1DAT                 (GPIO_BASE + 0x0064)
#define GPK1PUD                 (GPIO_BASE + 0x0068)
#define GPK1DRV                 (GPIO_BASE + 0x006C)

#define GPL2CON                 (GPIO_BASE + 0x0100)
#define GPL2DAT                 (GPIO_BASE + 0x0104)
#define GPL2PUD                 (GPIO_BASE + 0x0108)
#define GPL2DRV                 (GPIO_BASE + 0x010C)

#define GPX0CON                 (GPIO_BASE + 0x0C00)
#define GPX0DAT                 (GPIO_BASE + 0x0C04)
#define GPX0PUD                 (GPIO_BASE + 0x0C08)
#define GPX0DRV                 (GPIO_BASE + 0x0C0C)

/*~~~~~~~~~~~~*/
#define rGPD0CON                 (*(volatile unsigned long *) (GPIO_BASE + 0x00A0))
#define rGPD0DAT                 (*(volatile unsigned long *) (GPIO_BASE + 0x00A4))
#define rGPD0PUD                 (*(volatile unsigned long *) (GPIO_BASE + 0x00A8))
#define rGPD0DRV                 (*(volatile unsigned long *) (GPIO_BASE + 0x00AC))
#define rGPD0CONPDN              (*(volatile unsigned long *) (GPIO_BASE + 0x00B0))
#define rGPD0PUDPDN              (*(volatile unsigned long *) (GPIO_BASE + 0x00B4))

#define rGPK1CON                 (*(volatile unsigned long *) (GPIO_BASE + 0x0060))
#define rGPK1DAT                 (*(volatile unsigned long *) (GPIO_BASE + 0x0064))
#define rGPK1PUD                 (*(volatile unsigned long *) (GPIO_BASE + 0x0068))
#define rGPK1DRV                 (*(volatile unsigned long *) (GPIO_BASE + 0x006C))

#define rGPL2CON                 (*(volatile unsigned long *) (GPIO_BASE + 0x0100))
#define rGPL2DAT                 (*(volatile unsigned long *) (GPIO_BASE + 0x0104))
#define rGPL2PUD                 (*(volatile unsigned long *) (GPIO_BASE + 0x0108))
#define rGPL2DRV                 (*(volatile unsigned long *) (GPIO_BASE + 0x010C))

#define rGPX0CON                 (*(volatile unsigned long *) (GPIO_BASE + 0x0C00))
#define rGPX0DAT                 (*(volatile unsigned long *) (GPIO_BASE + 0x0C04))
#define rGPX0PUD                 (*(volatile unsigned long *) (GPIO_BASE + 0x0C08))
#define rGPX0DRV                 (*(volatile unsigned long *) (GPIO_BASE + 0x0C0C))

/* CMU */






/* PMU */





/* GIC  Generic Interrupt Controller */

#define GIC_BASE                    0x11400000

#define ICCICR_CPU0                 (GIC_BASE + 0x0000)
#define ICCPMR_CPU0                 (GIC_BASE + 0x0004)
#define ICCBPR_CPU0                 (GIC_BASE + 0x0008)
#define ICCIAR_CPU0                 (GIC_BASE + 0x000C)
#define ICCEOIR_CPU0                (GIC_BASE + 0x0010)
#define ICCRPR_CPU0                 (GIC_BASE + 0x0014)
#define ICCHPIR_CPU0                (GIC_BASE + 0x0018)
#define ICCABPR_CPU0                (GIC_BASE + 0x001C)
#define INTEG_EN_C_CPU0             (GIC_BASE + 0x0040)
#define INTERRUPT_OUT_CPU0          (GIC_BASE + 0x0044)
#define ICCIIDR                     (GIC_BASE + 0x00FC)
#define ICCICR_CPU1                 (GIC_BASE + 0x4000)
#define ICCPMR_CPU1                 (GIC_BASE + 0x4004)
#define ICCBPR_CPU1                 (GIC_BASE + 0x4008)
#define ICCIAR_CPU1                 (GIC_BASE + 0x400C)
#define ICCEOIR_CPU1                (GIC_BASE + 0x4010)
#define ICCRPR_CPU1                 (GIC_BASE + 0x4014)
#define ICCHPIR_CPU1                (GIC_BASE + 0x4018)
#define ICCABPR_CPU1                (GIC_BASE + 0x401C)
#define INTEG_C_EN_CPU1             (GIC_BASE + 0x4040)
#define INTERRUPT_OUT_CPU1          (GIC_BASE + 0x4044)
#define ICCICR_CPU2                 (GIC_BASE + 0x8000)
#define ICCPMR_CPU2                 (GIC_BASE + 0x8004)
#define ICCBPR_CPU2                 (GIC_BASE + 0x8008)
#define ICCIAR_CPU2                 (GIC_BASE + 0x800C)
#define ICCEOIR_CPU2                (GIC_BASE + 0x8010)
#define ICCRPR_CPU2                 (GIC_BASE + 0x8014)
#define ICCHPIR_CPU2                (GIC_BASE + 0x8018)
#define ICCABPR_CPU2                (GIC_BASE + 0x801C)
#define INTEG_C_EN_CPU2             (GIC_BASE + 0x8040)
#define INTERRUPT_OUT_CPU2          (GIC_BASE + 0x8044)
#define ICCICR_CPU3                 (GIC_BASE + 0xC000)
#define ICCPMR_CPU3                 (GIC_BASE + 0xC004)
#define ICCBPR_CPU3                 (GIC_BASE + 0xC008)
#define ICCIAR_CPU3                 (GIC_BASE + 0xC00C)
#define ICCEOIR_CPU3                (GIC_BASE + 0xC010)
#define ICCRPR_CPU3                 (GIC_BASE + 0xC014)
#define ICCHPIR_CPU3                (GIC_BASE + 0xC018)
#define ICCABPR_CPU3                (GIC_BASE + 0xC01C)
#define INTEG_C_EN_CPU3             (GIC_BASE + 0xC040)
#define INTERRUPT_OUT_CPU3          (GIC_BASE + 0xC044)
#define ICDDCR                      (GIC_BASE + 0x0000)
#define ICDICTR                     (GIC_BASE + 0x0004)
#define ICDIIDR                     (GIC_BASE + 0x0008)
#define ICDISR0_CPU0                (GIC_BASE + 0x0080)
#define ICDISR1                     (GIC_BASE + 0x0084)
#define ICDISR2                     (GIC_BASE + 0x0088)
#define ICDISR3                     (GIC_BASE + 0x008C)
#define ICDISR4                     (GIC_BASE + 0x0090)
#define ICDISER0_CPU0               (GIC_BASE + 0x0100)
#define ICDISER1                    (GIC_BASE + 0x0104)
#define ICDISER2                    (GIC_BASE + 0x0108)
#define ICDISER3                    (GIC_BASE + 0x010C)
#define ICDISER4                    (GIC_BASE + 0x0110)
#define ICDICER0_CPU0               (GIC_BASE + 0x0180)
#define ICDICER1                    (GIC_BASE + 0x0184)
#define ICDICER2                    (GIC_BASE + 0x0188)
#define ICDICER3                    (GIC_BASE + 0x018C)
#define ICDICER4                    (GIC_BASE + 0x0190)
#define ICDISPR0_CPU0               (GIC_BASE + 0x0200)
#define ICDISPR1                    (GIC_BASE + 0x0204)
#define ICDISPR2                    (GIC_BASE + 0x0208)
#define ICDISPR3                    (GIC_BASE + 0x020C)
#define ICDISPR4                    (GIC_BASE + 0x0210)
#define ICDICPR0_CPU0               (GIC_BASE + 0x0280)
#define ICDICPR1                    (GIC_BASE + 0x0284)
#define ICDICPR2                    (GIC_BASE + 0x0288)
#define ICDICPR3                    (GIC_BASE + 0x028C)
#define ICDICPR4                    (GIC_BASE + 0x0290)
#define ICDABR0_CPU0                (GIC_BASE + 0x0300)
#define ICDABR1                     (GIC_BASE + 0x0304)
#define ICDABR2                     (GIC_BASE + 0x0308)
#define ICDABR3                     (GIC_BASE + 0x030C)
#define ICDABR4                     (GIC_BASE + 0x0310)
#define ICDIPR0_CPU0                (GIC_BASE + 0x0400)
#define ICDIPR1_CPU0                (GIC_BASE + 0x0404)
#define ICDIPR2_CPU0                (GIC_BASE + 0x0408)
#define ICDIPR3_CPU0                (GIC_BASE + 0x040C)
#define ICDIPR4_CPU0                (GIC_BASE + 0x0410)
#define ICDIPR5_CPU0                (GIC_BASE + 0x0414)
#define ICDIPR6_CPU0                (GIC_BASE + 0x0418)
#define ICDIPR7_CPU0                (GIC_BASE + 0x041C)
#define ICDIPR8                     (GIC_BASE + 0x0420)
#define ICDIPR9                     (GIC_BASE + 0x0424)
#define ICDIPR10                    (GIC_BASE + 0x0428)
#define ICDIPR11                    (GIC_BASE + 0x042C)
#define ICDIPR12                    (GIC_BASE + 0x0430)
#define ICDIPR13                    (GIC_BASE + 0x0434)
#define ICDIPR14                    (GIC_BASE + 0x0438)
#define ICDIPR15                    (GIC_BASE + 0x043C)
#define ICDIPR16                    (GIC_BASE + 0x0440)
#define ICDIPR17                    (GIC_BASE + 0x0444)
#define ICDIPR18                    (GIC_BASE + 0x0448)
#define ICDIPR19                    (GIC_BASE + 0x044C)
#define ICDIPR20                    (GIC_BASE + 0x0450)
#define ICDIPR21                    (GIC_BASE + 0x0454)
#define ICDIPR22                    (GIC_BASE + 0x0458)
#define ICDIPR23                    (GIC_BASE + 0x045C)
#define ICDIPR24                    (GIC_BASE + 0x0460)
#define ICDIPR25                    (GIC_BASE + 0x0464)
#define ICDIPR26                    (GIC_BASE + 0x0468)
#define ICDIPR27                    (GIC_BASE + 0x046C)
#define ICDIPR28                    (GIC_BASE + 0x0470)
#define ICDIPR29                    (GIC_BASE + 0x0474)
#define ICDIPR30                    (GIC_BASE + 0x0478)
#define ICDIPR31                    (GIC_BASE + 0x047C)
#define ICDIPR32                    (GIC_BASE + 0x0480)
#define ICDIPR33                    (GIC_BASE + 0x0484)
#define ICDIPR34                    (GIC_BASE + 0x0488)
#define ICDIPR35                    (GIC_BASE + 0x048C)
#define ICDIPR36                    (GIC_BASE + 0x0490)
#define ICDIPR37                    (GIC_BASE + 0x0494)
#define ICDIPR38                    (GIC_BASE + 0x0498)
#define ICDIPR39                    (GIC_BASE + 0x049C)
#define ICDIPTR0_CPU0               (GIC_BASE + 0x0800)
#define ICDIPTR1_CPU0               (GIC_BASE + 0x0804)
#define ICDIPTR2_CPU0               (GIC_BASE + 0x0808)
#define ICDIPTR3_CPU0               (GIC_BASE + 0x080C)
#define ICDIPTR4_CPU0               (GIC_BASE + 0x0810)
#define ICDIPTR5_CPU0               (GIC_BASE + 0x0814)
#define ICDIPTR6_CPU0               (GIC_BASE + 0x0818)
#define ICDIPTR7_CPU0               (GIC_BASE + 0x081C)
#define ICDIPTR8                    (GIC_BASE + 0x0820)
#define ICDIPTR9                    (GIC_BASE + 0x0824)
#define ICDIPTR10                   (GIC_BASE + 0x0828)
#define ICDIPTR11                   (GIC_BASE + 0x082C)
#define ICDIPTR12                   (GIC_BASE + 0x0830)
#define ICDIPTR13                   (GIC_BASE + 0x0834)
#define ICDIPTR14                   (GIC_BASE + 0x0838)
#define ICDIPTR15                   (GIC_BASE + 0x083C)
#define ICDIPTR16                   (GIC_BASE + 0x0840)
#define ICDIPTR17                   (GIC_BASE + 0x0844)
#define ICDIPTR18                   (GIC_BASE + 0x0848)
#define ICDIPTR19                   (GIC_BASE + 0x084C)
#define ICDIPTR20                   (GIC_BASE + 0x0850)
#define ICDIPTR21                   (GIC_BASE + 0x0854)
#define ICDIPTR22                   (GIC_BASE + 0x0858)
#define ICDIPTR23                   (GIC_BASE + 0x085C)
#define ICDIPTR24                   (GIC_BASE + 0x0860)
#define ICDIPTR25                   (GIC_BASE + 0x0864)
#define ICDIPTR26                   (GIC_BASE + 0x0868)
#define ICDIPTR27                   (GIC_BASE + 0x086C)
#define ICDIPTR28                   (GIC_BASE + 0x0870)
#define ICDIPTR29                   (GIC_BASE + 0x0874)
#define ICDIPTR30                   (GIC_BASE + 0x0878)
#define ICDIPTR31                   (GIC_BASE + 0x087C)
#define ICDIPTR32                   (GIC_BASE + 0x0880)
#define ICDIPTR33                   (GIC_BASE + 0x0884)
#define ICDIPTR34                   (GIC_BASE + 0x0888)
#define ICDIPTR35                   (GIC_BASE + 0x088C)
#define ICDIPTR36                   (GIC_BASE + 0x0890)
#define ICDIPTR37                   (GIC_BASE + 0x0894)
#define ICDIPTR38                   (GIC_BASE + 0x0898)
#define ICDIPTR39                   (GIC_BASE + 0x089C)
#define ICDICFR0_CPU0               (GIC_BASE + 0x0C00)
#define ICDICFR1_CPU0               (GIC_BASE + 0x0C04)
#define ICDICFR2                    (GIC_BASE + 0x0C08)
#define ICDICFR3                    (GIC_BASE + 0x0C0C)
#define ICDICFR4                    (GIC_BASE + 0x0C10)
#define ICDICFR5                    (GIC_BASE + 0x0C14)
#define ICDICFR6                    (GIC_BASE + 0x0C18)
#define ICDICFR7                    (GIC_BASE + 0x0C1C)
#define ICDICFR8                    (GIC_BASE + 0x0C20)
#define ICDICFR9                    (GIC_BASE + 0x0C24)
#define PPI_STATUS_CPU0             (GIC_BASE + 0x0D00)
#define SPI_STATUS0                 (GIC_BASE + 0x0D04)
#define SPI_STATUS1                 (GIC_BASE + 0x0D08)
#define SPI_STATUS2                 (GIC_BASE + 0x0D0C)
#define SPI_STATUS3                 (GIC_BASE + 0x0D10)
#define ICDSGIR                     (GIC_BASE + 0x0F00)
#define ICDISR0_CPU1                (GIC_BASE + 0x4080)
#define ICDISER0_CPU1               (GIC_BASE + 0x4100)
#define ICDICER0_CPU1               (GIC_BASE + 0x4180)
#define ICDISPR0_CPU1               (GIC_BASE + 0x4200)
#define ICDICPR0_CPU1               (GIC_BASE + 0x4280)
#define ICDABR0_CPU1                (GIC_BASE + 0x4300)
#define ICDIPR0_CPU1                (GIC_BASE + 0x4400)
#define ICDIPR1_CPU1                (GIC_BASE + 0x4404)
#define ICDIPR2_CPU1                (GIC_BASE + 0x4408)
#define ICDIPR3_CPU1                (GIC_BASE + 0x440C)
#define ICDIPR4_CPU1                (GIC_BASE + 0x4410)
#define ICDIPR5_CPU1                (GIC_BASE + 0x4414)
#define ICDIPR6_CPU1                (GIC_BASE + 0x4418)
#define ICDIPR7_CPU1                (GIC_BASE + 0x441C)
#define ICDIPTR0_CPU1               (GIC_BASE + 0x4800)
#define ICDIPTR1_CPU1               (GIC_BASE + 0x4804)
#define ICDIPTR2_CPU1               (GIC_BASE + 0x4808)
#define ICDIPTR3_CPU1               (GIC_BASE + 0x480C)
#define ICDIPTR4_CPU1               (GIC_BASE + 0x4810)
#define ICDIPTR5_CPU1               (GIC_BASE + 0x4814)
#define ICDIPTR6_CPU1               (GIC_BASE + 0x4818)
#define ICDIPTR7_CPU1               (GIC_BASE + 0x481C)
#define ICDICFR0_CPU1               (GIC_BASE + 0x4C00)
#define ICDICFR1_CPU1               (GIC_BASE + 0x4C04)
#define PPI_STATUS_CPU1             (GIC_BASE + 0x4D00)
#define ICDISR0_CPU2                (GIC_BASE + 0x8080)
#define ICDISER0_CPU2               (GIC_BASE + 0x8100)
#define ICDICER0_CPU2               (GIC_BASE + 0x8180)
#define ICDISPR0_CPU2               (GIC_BASE + 0x8200)
#define ICDICPR0_CPU2               (GIC_BASE + 0x8280)
#define ICDABR0_CPU2                (GIC_BASE + 0x8300)
#define ICDIPR0_CPU2                (GIC_BASE + 0x8400)
#define ICDIPR1_CPU2                (GIC_BASE + 0x8404)
#define ICDIPR2_CPU2                (GIC_BASE + 0x8408)
#define ICDIPR3_CPU2                (GIC_BASE + 0x840C)
#define ICDIPR4_CPU2                (GIC_BASE + 0x8410)
#define ICDIPR5_CPU2                (GIC_BASE + 0x8414)
#define ICDIPR6_CPU2                (GIC_BASE + 0x8418)
#define ICDIPR7_CPU2                (GIC_BASE + 0x841C)
#define ICDIPTR0_CPU2               (GIC_BASE + 0x8800)
#define ICDIPTR1_CPU2               (GIC_BASE + 0x8804)
#define ICDIPTR2_CPU2               (GIC_BASE + 0x8808)
#define ICDIPTR3_CPU2               (GIC_BASE + 0x880C)
#define ICDIPTR4_CPU2               (GIC_BASE + 0x8810)
#define ICDIPTR5_CPU2               (GIC_BASE + 0x8814)
#define ICDIPTR6_CPU2               (GIC_BASE + 0x8818)
#define ICDIPTR7_CPU2               (GIC_BASE + 0x881C)
#define ICDICFR0_CPU2               (GIC_BASE + 0x8C00)
#define ICDICFR1_CPU2               (GIC_BASE + 0x8C04)
#define PPI_STATUS_CPU2             (GIC_BASE + 0x8D00)
#define ICDISR0_CPU3                (GIC_BASE + 0xC080)
#define ICDISER0_CPU3               (GIC_BASE + 0xC100)
#define ICDICER0_CPU3               (GIC_BASE + 0xC180)
#define ICDISPR0_CPU3               (GIC_BASE + 0xC200)
#define ICDICPR0_CPU3               (GIC_BASE + 0xC280)
#define ICDABR0_CPU3                (GIC_BASE + 0xC300)
#define ICDIPR0_CPU3                (GIC_BASE + 0xC400)
#define ICDIPR1_CPU3                (GIC_BASE + 0xC404)
#define ICDIPR2_CPU3                (GIC_BASE + 0xC408)
#define ICDIPR3_CPU3                (GIC_BASE + 0xC40C)
#define ICDIPR4_CPU3                (GIC_BASE + 0xC410)
#define ICDIPR5_CPU3                (GIC_BASE + 0xC414)
#define ICDIPR6_CPU3                (GIC_BASE + 0xC418)
#define ICDIPR7_CPU3                (GIC_BASE + 0xC41C)
#define ICDIPTR0_CPU3               (GIC_BASE + 0xC800)
#define ICDIPTR1_CPU3               (GIC_BASE + 0xC804)
#define ICDIPTR2_CPU3               (GIC_BASE + 0xC808)
#define ICDIPTR3_CPU3               (GIC_BASE + 0xC80C)
#define ICDIPTR4_CPU3               (GIC_BASE + 0xC810)
#define ICDIPTR5_CPU3               (GIC_BASE + 0xC814)
#define ICDIPTR6_CPU3               (GIC_BASE + 0xC818)
#define ICDIPTR7_CPU3               (GIC_BASE + 0xC81C)
#define ICDICFR0_CPU3               (GIC_BASE + 0xCC00)
#define ICDICFR1_CPU3               (GIC_BASE + 0xCC04)
#define PPI_STATUS_CPU3             (GIC_BASE + 0xCD00)


/*~~~~~~~~~~~~~~~~~~*/
#define rICCICR_CPU0                 (*(volatile unsigned long *) (GIC_BASE + 0x0000))
#define rICCPMR_CPU0                 (*(volatile unsigned long *) (GIC_BASE + 0x0004))
#define rICCBPR_CPU0                 (*(volatile unsigned long *) (GIC_BASE + 0x0008))
#define rICCIAR_CPU0                 (*(volatile unsigned long *) (GIC_BASE + 0x000C))
#define rICCEOIR_CPU0                (*(volatile unsigned long *) (GIC_BASE + 0x0010))
#define rICCRPR_CPU0                 (*(volatile unsigned long *) (GIC_BASE + 0x0014))
#define rICCHPIR_CPU0                (*(volatile unsigned long *) (GIC_BASE + 0x0018))
#define rICCABPR_CPU0                (*(volatile unsigned long *) (GIC_BASE + 0x001C))
#define rINTEG_EN_C_CPU0             (*(volatile unsigned long *) (GIC_BASE + 0x0040))
#define rINTERRUPT_OUT_CPU0          (*(volatile unsigned long *) (GIC_BASE + 0x0044))
#define rICCIIDR                     (*(volatile unsigned long *) (GIC_BASE + 0x00FC))
#define rICCICR_CPU1                 (*(volatile unsigned long *) (GIC_BASE + 0x4000))
#define rICCPMR_CPU1                 (*(volatile unsigned long *) (GIC_BASE + 0x4004))
#define rICCBPR_CPU1                 (*(volatile unsigned long *) (GIC_BASE + 0x4008))
#define rICCIAR_CPU1                 (*(volatile unsigned long *) (GIC_BASE + 0x400C))
#define rICCEOIR_CPU1                (*(volatile unsigned long *) (GIC_BASE + 0x4010))
#define rICCRPR_CPU1                 (*(volatile unsigned long *) (GIC_BASE + 0x4014))
#define rICCHPIR_CPU1                (*(volatile unsigned long *) (GIC_BASE + 0x4018))
#define rICCABPR_CPU1                (*(volatile unsigned long *) (GIC_BASE + 0x401C))
#define rINTEG_C_EN_CPU1             (*(volatile unsigned long *) (GIC_BASE + 0x4040))
#define rINTERRUPT_OUT_CPU1          (*(volatile unsigned long *) (GIC_BASE + 0x4044))
#define rICCICR_CPU2                 (*(volatile unsigned long *) (GIC_BASE + 0x8000))
#define rICCPMR_CPU2                 (*(volatile unsigned long *) (GIC_BASE + 0x8004))
#define rICCBPR_CPU2                 (*(volatile unsigned long *) (GIC_BASE + 0x8008))
#define rICCIAR_CPU2                 (*(volatile unsigned long *) (GIC_BASE + 0x800C))
#define rICCEOIR_CPU2                (*(volatile unsigned long *) (GIC_BASE + 0x8010))
#define rICCRPR_CPU2                 (*(volatile unsigned long *) (GIC_BASE + 0x8014))
#define rICCHPIR_CPU2                (*(volatile unsigned long *) (GIC_BASE + 0x8018))
#define rICCABPR_CPU2                (*(volatile unsigned long *) (GIC_BASE + 0x801C))
#define rINTEG_C_EN_CPU2             (*(volatile unsigned long *) (GIC_BASE + 0x8040))
#define rINTERRUPT_OUT_CPU2          (*(volatile unsigned long *) (GIC_BASE + 0x8044))
#define rICCICR_CPU3                 (*(volatile unsigned long *) (GIC_BASE + 0xC000))
#define rICCPMR_CPU3                 (*(volatile unsigned long *) (GIC_BASE + 0xC004))
#define rICCBPR_CPU3                 (*(volatile unsigned long *) (GIC_BASE + 0xC008))
#define rICCIAR_CPU3                 (*(volatile unsigned long *) (GIC_BASE + 0xC00C))
#define rICCEOIR_CPU3                (*(volatile unsigned long *) (GIC_BASE + 0xC010))
#define rICCRPR_CPU3                 (*(volatile unsigned long *) (GIC_BASE + 0xC014))
#define rICCHPIR_CPU3                (*(volatile unsigned long *) (GIC_BASE + 0xC018))
#define rICCABPR_CPU3                (*(volatile unsigned long *) (GIC_BASE + 0xC01C))
#define rINTEG_C_EN_CPU3             (*(volatile unsigned long *) (GIC_BASE + 0xC040))
#define rINTERRUPT_OUT_CPU3          (*(volatile unsigned long *) (GIC_BASE + 0xC044))
#define rICDDCR                      (*(volatile unsigned long *) (GIC_BASE + 0x0000))
#define rICDICTR                     (*(volatile unsigned long *) (GIC_BASE + 0x0004))
#define rICDIIDR                     (*(volatile unsigned long *) (GIC_BASE + 0x0008))
#define rICDISR0_CPU0                (*(volatile unsigned long *) (GIC_BASE + 0x0080))
#define rICDISR1                     (*(volatile unsigned long *) (GIC_BASE + 0x0084))
#define rICDISR2                     (*(volatile unsigned long *) (GIC_BASE + 0x0088))
#define rICDISR3                     (*(volatile unsigned long *) (GIC_BASE + 0x008C))
#define rICDISR4                     (*(volatile unsigned long *) (GIC_BASE + 0x0090))
#define rICDISER0_CPU0               (*(volatile unsigned long *) (GIC_BASE + 0x0100))
#define rICDISER1                    (*(volatile unsigned long *) (GIC_BASE + 0x0104))
#define rICDISER2                    (*(volatile unsigned long *) (GIC_BASE + 0x0108))
#define rICDISER3                    (*(volatile unsigned long *) (GIC_BASE + 0x010C))
#define rICDISER4                    (*(volatile unsigned long *) (GIC_BASE + 0x0110))
#define rICDICER0_CPU0               (*(volatile unsigned long *) (GIC_BASE + 0x0180))
#define rICDICER1                    (*(volatile unsigned long *) (GIC_BASE + 0x0184))
#define rICDICER2                    (*(volatile unsigned long *) (GIC_BASE + 0x0188))
#define rICDICER3                    (*(volatile unsigned long *) (GIC_BASE + 0x018C))
#define rICDICER4                    (*(volatile unsigned long *) (GIC_BASE + 0x0190))
#define rICDISPR0_CPU0               (*(volatile unsigned long *) (GIC_BASE + 0x0200))
#define rICDISPR1                    (*(volatile unsigned long *) (GIC_BASE + 0x0204))
#define rICDISPR2                    (*(volatile unsigned long *) (GIC_BASE + 0x0208))
#define rICDISPR3                    (*(volatile unsigned long *) (GIC_BASE + 0x020C))
#define rICDISPR4                    (*(volatile unsigned long *) (GIC_BASE + 0x0210))
#define rICDICPR0_CPU0               (*(volatile unsigned long *) (GIC_BASE + 0x0280))
#define rICDICPR1                    (*(volatile unsigned long *) (GIC_BASE + 0x0284))
#define rICDICPR2                    (*(volatile unsigned long *) (GIC_BASE + 0x0288))
#define rICDICPR3                    (*(volatile unsigned long *) (GIC_BASE + 0x028C))
#define rICDICPR4                    (*(volatile unsigned long *) (GIC_BASE + 0x0290))
#define rICDABR0_CPU0                (*(volatile unsigned long *) (GIC_BASE + 0x0300))
#define rICDABR1                     (*(volatile unsigned long *) (GIC_BASE + 0x0304))
#define rICDABR2                     (*(volatile unsigned long *) (GIC_BASE + 0x0308))
#define rICDABR3                     (*(volatile unsigned long *) (GIC_BASE + 0x030C))
#define rICDABR4                     (*(volatile unsigned long *) (GIC_BASE + 0x0310))
#define rICDIPR0_CPU0                (*(volatile unsigned long *) (GIC_BASE + 0x0400))
#define rICDIPR1_CPU0                (*(volatile unsigned long *) (GIC_BASE + 0x0404))
#define rICDIPR2_CPU0                (*(volatile unsigned long *) (GIC_BASE + 0x0408))
#define rICDIPR3_CPU0                (*(volatile unsigned long *) (GIC_BASE + 0x040C))
#define rICDIPR4_CPU0                (*(volatile unsigned long *) (GIC_BASE + 0x0410))
#define rICDIPR5_CPU0                (*(volatile unsigned long *) (GIC_BASE + 0x0414))
#define rICDIPR6_CPU0                (*(volatile unsigned long *) (GIC_BASE + 0x0418))
#define rICDIPR7_CPU0                (*(volatile unsigned long *) (GIC_BASE + 0x041C))
#define rICDIPR8                     (*(volatile unsigned long *) (GIC_BASE + 0x0420))
#define rICDIPR9                     (*(volatile unsigned long *) (GIC_BASE + 0x0424))
#define rICDIPR10                    (*(volatile unsigned long *) (GIC_BASE + 0x0428))
#define rICDIPR11                    (*(volatile unsigned long *) (GIC_BASE + 0x042C))
#define rICDIPR12                    (*(volatile unsigned long *) (GIC_BASE + 0x0430))
#define rICDIPR13                    (*(volatile unsigned long *) (GIC_BASE + 0x0434))
#define rICDIPR14                    (*(volatile unsigned long *) (GIC_BASE + 0x0438))
#define rICDIPR15                    (*(volatile unsigned long *) (GIC_BASE + 0x043C))
#define rICDIPR16                    (*(volatile unsigned long *) (GIC_BASE + 0x0440))
#define rICDIPR17                    (*(volatile unsigned long *) (GIC_BASE + 0x0444))
#define rICDIPR18                    (*(volatile unsigned long *) (GIC_BASE + 0x0448))
#define rICDIPR19                    (*(volatile unsigned long *) (GIC_BASE + 0x044C))
#define rICDIPR20                    (*(volatile unsigned long *) (GIC_BASE + 0x0450))
#define rICDIPR21                    (*(volatile unsigned long *) (GIC_BASE + 0x0454))
#define rICDIPR22                    (*(volatile unsigned long *) (GIC_BASE + 0x0458))
#define rICDIPR23                    (*(volatile unsigned long *) (GIC_BASE + 0x045C))
#define rICDIPR24                    (*(volatile unsigned long *) (GIC_BASE + 0x0460))
#define rICDIPR25                    (*(volatile unsigned long *) (GIC_BASE + 0x0464))
#define rICDIPR26                    (*(volatile unsigned long *) (GIC_BASE + 0x0468))
#define rICDIPR27                    (*(volatile unsigned long *) (GIC_BASE + 0x046C))
#define rICDIPR28                    (*(volatile unsigned long *) (GIC_BASE + 0x0470))
#define rICDIPR29                    (*(volatile unsigned long *) (GIC_BASE + 0x0474))
#define rICDIPR30                    (*(volatile unsigned long *) (GIC_BASE + 0x0478))
#define rICDIPR31                    (*(volatile unsigned long *) (GIC_BASE + 0x047C))
#define rICDIPR32                    (*(volatile unsigned long *) (GIC_BASE + 0x0480))
#define rICDIPR33                    (*(volatile unsigned long *) (GIC_BASE + 0x0484))
#define rICDIPR34                    (*(volatile unsigned long *) (GIC_BASE + 0x0488))
#define rICDIPR35                    (*(volatile unsigned long *) (GIC_BASE + 0x048C))
#define rICDIPR36                    (*(volatile unsigned long *) (GIC_BASE + 0x0490))
#define rICDIPR37                    (*(volatile unsigned long *) (GIC_BASE + 0x0494))
#define rICDIPR38                    (*(volatile unsigned long *) (GIC_BASE + 0x0498))
#define rICDIPR39                    (*(volatile unsigned long *) (GIC_BASE + 0x049C))
#define rICDIPTR0_CPU0               (*(volatile unsigned long *) (GIC_BASE + 0x0800))
#define rICDIPTR1_CPU0               (*(volatile unsigned long *) (GIC_BASE + 0x0804))
#define rICDIPTR2_CPU0               (*(volatile unsigned long *) (GIC_BASE + 0x0808))
#define rICDIPTR3_CPU0               (*(volatile unsigned long *) (GIC_BASE + 0x080C))
#define rICDIPTR4_CPU0               (*(volatile unsigned long *) (GIC_BASE + 0x0810))
#define rICDIPTR5_CPU0               (*(volatile unsigned long *) (GIC_BASE + 0x0814))
#define rICDIPTR6_CPU0               (*(volatile unsigned long *) (GIC_BASE + 0x0818))
#define rICDIPTR7_CPU0               (*(volatile unsigned long *) (GIC_BASE + 0x081C))
#define rICDIPTR8                    (*(volatile unsigned long *) (GIC_BASE + 0x0820))
#define rICDIPTR9                    (*(volatile unsigned long *) (GIC_BASE + 0x0824))
#define rICDIPTR10                   (*(volatile unsigned long *) (GIC_BASE + 0x0828))
#define rICDIPTR11                   (*(volatile unsigned long *) (GIC_BASE + 0x082C))
#define rICDIPTR12                   (*(volatile unsigned long *) (GIC_BASE + 0x0830))
#define rICDIPTR13                   (*(volatile unsigned long *) (GIC_BASE + 0x0834))
#define rICDIPTR14                   (*(volatile unsigned long *) (GIC_BASE + 0x0838))
#define rICDIPTR15                   (*(volatile unsigned long *) (GIC_BASE + 0x083C))
#define rICDIPTR16                   (*(volatile unsigned long *) (GIC_BASE + 0x0840))
#define rICDIPTR17                   (*(volatile unsigned long *) (GIC_BASE + 0x0844))
#define rICDIPTR18                   (*(volatile unsigned long *) (GIC_BASE + 0x0848))
#define rICDIPTR19                   (*(volatile unsigned long *) (GIC_BASE + 0x084C))
#define rICDIPTR20                   (*(volatile unsigned long *) (GIC_BASE + 0x0850))
#define rICDIPTR21                   (*(volatile unsigned long *) (GIC_BASE + 0x0854))
#define rICDIPTR22                   (*(volatile unsigned long *) (GIC_BASE + 0x0858))
#define rICDIPTR23                   (*(volatile unsigned long *) (GIC_BASE + 0x085C))
#define rICDIPTR24                   (*(volatile unsigned long *) (GIC_BASE + 0x0860))
#define rICDIPTR25                   (*(volatile unsigned long *) (GIC_BASE + 0x0864))
#define rICDIPTR26                   (*(volatile unsigned long *) (GIC_BASE + 0x0868))
#define rICDIPTR27                   (*(volatile unsigned long *) (GIC_BASE + 0x086C))
#define rICDIPTR28                   (*(volatile unsigned long *) (GIC_BASE + 0x0870))
#define rICDIPTR29                   (*(volatile unsigned long *) (GIC_BASE + 0x0874))
#define rICDIPTR30                   (*(volatile unsigned long *) (GIC_BASE + 0x0878))
#define rICDIPTR31                   (*(volatile unsigned long *) (GIC_BASE + 0x087C))
#define rICDIPTR32                   (*(volatile unsigned long *) (GIC_BASE + 0x0880))
#define rICDIPTR33                   (*(volatile unsigned long *) (GIC_BASE + 0x0884))
#define rICDIPTR34                   (*(volatile unsigned long *) (GIC_BASE + 0x0888))
#define rICDIPTR35                   (*(volatile unsigned long *) (GIC_BASE + 0x088C))
#define rICDIPTR36                   (*(volatile unsigned long *) (GIC_BASE + 0x0890))
#define rICDIPTR37                   (*(volatile unsigned long *) (GIC_BASE + 0x0894))
#define rICDIPTR38                   (*(volatile unsigned long *) (GIC_BASE + 0x0898))
#define rICDIPTR39                   (*(volatile unsigned long *) (GIC_BASE + 0x089C))
#define rICDICFR0_CPU0               (*(volatile unsigned long *) (GIC_BASE + 0x0C00))
#define rICDICFR1_CPU0               (*(volatile unsigned long *) (GIC_BASE + 0x0C04))
#define rICDICFR2                    (*(volatile unsigned long *) (GIC_BASE + 0x0C08))
#define rICDICFR3                    (*(volatile unsigned long *) (GIC_BASE + 0x0C0C))
#define rICDICFR4                    (*(volatile unsigned long *) (GIC_BASE + 0x0C10))
#define rICDICFR5                    (*(volatile unsigned long *) (GIC_BASE + 0x0C14))
#define rICDICFR6                    (*(volatile unsigned long *) (GIC_BASE + 0x0C18))
#define rICDICFR7                    (*(volatile unsigned long *) (GIC_BASE + 0x0C1C))
#define rICDICFR8                    (*(volatile unsigned long *) (GIC_BASE + 0x0C20))
#define rICDICFR9                    (*(volatile unsigned long *) (GIC_BASE + 0x0C24))
#define rPPI_STATUS_CPU0             (*(volatile unsigned long *) (GIC_BASE + 0x0D00))
#define rSPI_STATUS0                 (*(volatile unsigned long *) (GIC_BASE + 0x0D04))
#define rSPI_STATUS1                 (*(volatile unsigned long *) (GIC_BASE + 0x0D08))
#define rSPI_STATUS2                 (*(volatile unsigned long *) (GIC_BASE + 0x0D0C))
#define rSPI_STATUS3                 (*(volatile unsigned long *) (GIC_BASE + 0x0D10))
#define rICDSGIR                     (*(volatile unsigned long *) (GIC_BASE + 0x0F00))
#define rICDISR0_CPU1                (*(volatile unsigned long *) (GIC_BASE + 0x4080))
#define rICDISER0_CPU1               (*(volatile unsigned long *) (GIC_BASE + 0x4100))
#define rICDICER0_CPU1               (*(volatile unsigned long *) (GIC_BASE + 0x4180))
#define rICDISPR0_CPU1               (*(volatile unsigned long *) (GIC_BASE + 0x4200))
#define rICDICPR0_CPU1               (*(volatile unsigned long *) (GIC_BASE + 0x4280))
#define rICDABR0_CPU1                (*(volatile unsigned long *) (GIC_BASE + 0x4300))
#define rICDIPR0_CPU1                (*(volatile unsigned long *) (GIC_BASE + 0x4400))
#define rICDIPR1_CPU1                (*(volatile unsigned long *) (GIC_BASE + 0x4404))
#define rICDIPR2_CPU1                (*(volatile unsigned long *) (GIC_BASE + 0x4408))
#define rICDIPR3_CPU1                (*(volatile unsigned long *) (GIC_BASE + 0x440C))
#define rICDIPR4_CPU1                (*(volatile unsigned long *) (GIC_BASE + 0x4410))
#define rICDIPR5_CPU1                (*(volatile unsigned long *) (GIC_BASE + 0x4414))
#define rICDIPR6_CPU1                (*(volatile unsigned long *) (GIC_BASE + 0x4418))
#define rICDIPR7_CPU1                (*(volatile unsigned long *) (GIC_BASE + 0x441C))
#define rICDIPTR0_CPU1               (*(volatile unsigned long *) (GIC_BASE + 0x4800))
#define rICDIPTR1_CPU1               (*(volatile unsigned long *) (GIC_BASE + 0x4804))
#define rICDIPTR2_CPU1               (*(volatile unsigned long *) (GIC_BASE + 0x4808))
#define rICDIPTR3_CPU1               (*(volatile unsigned long *) (GIC_BASE + 0x480C))
#define rICDIPTR4_CPU1               (*(volatile unsigned long *) (GIC_BASE + 0x4810))
#define rICDIPTR5_CPU1               (*(volatile unsigned long *) (GIC_BASE + 0x4814))
#define rICDIPTR6_CPU1               (*(volatile unsigned long *) (GIC_BASE + 0x4818))
#define rICDIPTR7_CPU1               (*(volatile unsigned long *) (GIC_BASE + 0x481C))
#define rICDICFR0_CPU1               (*(volatile unsigned long *) (GIC_BASE + 0x4C00))
#define rICDICFR1_CPU1               (*(volatile unsigned long *) (GIC_BASE + 0x4C04))
#define rPPI_STATUS_CPU1             (*(volatile unsigned long *) (GIC_BASE + 0x4D00))
#define rICDISR0_CPU2                (*(volatile unsigned long *) (GIC_BASE + 0x8080))
#define rICDISER0_CPU2               (*(volatile unsigned long *) (GIC_BASE + 0x8100))
#define rICDICER0_CPU2               (*(volatile unsigned long *) (GIC_BASE + 0x8180))
#define rICDISPR0_CPU2               (*(volatile unsigned long *) (GIC_BASE + 0x8200))
#define rICDICPR0_CPU2               (*(volatile unsigned long *) (GIC_BASE + 0x8280))
#define rICDABR0_CPU2                (*(volatile unsigned long *) (GIC_BASE + 0x8300))
#define rICDIPR0_CPU2                (*(volatile unsigned long *) (GIC_BASE + 0x8400))
#define rICDIPR1_CPU2                (*(volatile unsigned long *) (GIC_BASE + 0x8404))
#define rICDIPR2_CPU2                (*(volatile unsigned long *) (GIC_BASE + 0x8408))
#define rICDIPR3_CPU2                (*(volatile unsigned long *) (GIC_BASE + 0x840C))
#define rICDIPR4_CPU2                (*(volatile unsigned long *) (GIC_BASE + 0x8410))
#define rICDIPR5_CPU2                (*(volatile unsigned long *) (GIC_BASE + 0x8414))
#define rICDIPR6_CPU2                (*(volatile unsigned long *) (GIC_BASE + 0x8418))
#define rICDIPR7_CPU2                (*(volatile unsigned long *) (GIC_BASE + 0x841C))
#define rICDIPTR0_CPU2               (*(volatile unsigned long *) (GIC_BASE + 0x8800))
#define rICDIPTR1_CPU2               (*(volatile unsigned long *) (GIC_BASE + 0x8804))
#define rICDIPTR2_CPU2               (*(volatile unsigned long *) (GIC_BASE + 0x8808))
#define rICDIPTR3_CPU2               (*(volatile unsigned long *) (GIC_BASE + 0x880C))
#define rICDIPTR4_CPU2               (*(volatile unsigned long *) (GIC_BASE + 0x8810))
#define rICDIPTR5_CPU2               (*(volatile unsigned long *) (GIC_BASE + 0x8814))
#define rICDIPTR6_CPU2               (*(volatile unsigned long *) (GIC_BASE + 0x8818))
#define rICDIPTR7_CPU2               (*(volatile unsigned long *) (GIC_BASE + 0x881C))
#define rICDICFR0_CPU2               (*(volatile unsigned long *) (GIC_BASE + 0x8C00))
#define rICDICFR1_CPU2               (*(volatile unsigned long *) (GIC_BASE + 0x8C04))
#define rPPI_STATUS_CPU2             (*(volatile unsigned long *) (GIC_BASE + 0x8D00))
#define rICDISR0_CPU3                (*(volatile unsigned long *) (GIC_BASE + 0xC080))
#define rICDISER0_CPU3               (*(volatile unsigned long *) (GIC_BASE + 0xC100))
#define rICDICER0_CPU3               (*(volatile unsigned long *) (GIC_BASE + 0xC180))
#define rICDISPR0_CPU3               (*(volatile unsigned long *) (GIC_BASE + 0xC200))
#define rICDICPR0_CPU3               (*(volatile unsigned long *) (GIC_BASE + 0xC280))
#define rICDABR0_CPU3                (*(volatile unsigned long *) (GIC_BASE + 0xC300))
#define rICDIPR0_CPU3                (*(volatile unsigned long *) (GIC_BASE + 0xC400))
#define rICDIPR1_CPU3                (*(volatile unsigned long *) (GIC_BASE + 0xC404))
#define rICDIPR2_CPU3                (*(volatile unsigned long *) (GIC_BASE + 0xC408))
#define rICDIPR3_CPU3                (*(volatile unsigned long *) (GIC_BASE + 0xC40C))
#define rICDIPR4_CPU3                (*(volatile unsigned long *) (GIC_BASE + 0xC410))
#define rICDIPR5_CPU3                (*(volatile unsigned long *) (GIC_BASE + 0xC414))
#define rICDIPR6_CPU3                (*(volatile unsigned long *) (GIC_BASE + 0xC418))
#define rICDIPR7_CPU3                (*(volatile unsigned long *) (GIC_BASE + 0xC41C))
#define rICDIPTR0_CPU3               (*(volatile unsigned long *) (GIC_BASE + 0xC800))
#define rICDIPTR1_CPU3               (*(volatile unsigned long *) (GIC_BASE + 0xC804))
#define rICDIPTR2_CPU3               (*(volatile unsigned long *) (GIC_BASE + 0xC808))
#define rICDIPTR3_CPU3               (*(volatile unsigned long *) (GIC_BASE + 0xC80C))
#define rICDIPTR4_CPU3               (*(volatile unsigned long *) (GIC_BASE + 0xC810))
#define rICDIPTR5_CPU3               (*(volatile unsigned long *) (GIC_BASE + 0xC814))
#define rICDIPTR6_CPU3               (*(volatile unsigned long *) (GIC_BASE + 0xC818))
#define rICDIPTR7_CPU3               (*(volatile unsigned long *) (GIC_BASE + 0xC81C))
#define rICDICFR0_CPU3               (*(volatile unsigned long *) (GIC_BASE + 0xCC00))
#define rICDICFR1_CPU3               (*(volatile unsigned long *) (GIC_BASE + 0xCC04))
#define rPPI_STATUS_CPU3             (*(volatile unsigned long *) (GIC_BASE + 0xCD00))






/* IC  Interrupt combiner  */

#define IC_BASE               0x10440000

#define IESR0                 (IC_BASE + 0x0000)
#define IECR0                 (IC_BASE + 0x0004)
#define ISTR0                 (IC_BASE + 0x0008)
#define IMSR0                 (IC_BASE + 0x000C)
#define IESR1                 (IC_BASE + 0x0010)
#define IECR1                 (IC_BASE + 0x0014)
#define ISTR1                 (IC_BASE + 0x0018)
#define IMSR1                 (IC_BASE + 0x001C)
#define IESR2                 (IC_BASE + 0x0020)
#define IECR2                 (IC_BASE + 0x0024)
#define ISTR2                 (IC_BASE + 0x0028)
#define IMSR2                 (IC_BASE + 0x002C)
#define IESR3                 (IC_BASE + 0x0030)
#define IECR3                 (IC_BASE + 0x0034)
#define ISTR3                 (IC_BASE + 0x0038)
#define IMSR3                 (IC_BASE + 0x003C)
#define IESR4                 (IC_BASE + 0x0040)
#define IECR4                 (IC_BASE + 0x0044)
#define ISTR4                 (IC_BASE + 0x0048)
#define IMSR4                 (IC_BASE + 0x004C)
#define CIPSR0                (IC_BASE + 0x0100)

/*~~~~~~~~~~~~*/
#define rIESR0                 (*(volatile unsigned long *) (IC_BASE + 0x0000))
#define rIECR0                 (*(volatile unsigned long *) (IC_BASE + 0x0004))
#define rISTR0                 (*(volatile unsigned long *) (IC_BASE + 0x0008))
#define rIMSR0                 (*(volatile unsigned long *) (IC_BASE + 0x000C))
#define rIESR1                 (*(volatile unsigned long *) (IC_BASE + 0x0010))
#define rIECR1                 (*(volatile unsigned long *) (IC_BASE + 0x0014))
#define rISTR1                 (*(volatile unsigned long *) (IC_BASE + 0x0018))
#define rIMSR1                 (*(volatile unsigned long *) (IC_BASE + 0x001C))
#define rIESR2                 (*(volatile unsigned long *) (IC_BASE + 0x0020))
#define rIECR2                 (*(volatile unsigned long *) (IC_BASE + 0x0024))
#define rISTR2                 (*(volatile unsigned long *) (IC_BASE + 0x0028))
#define rIMSR2                 (*(volatile unsigned long *) (IC_BASE + 0x002C))
#define rIESR3                 (*(volatile unsigned long *) (IC_BASE + 0x0030))
#define rIECR3                 (*(volatile unsigned long *) (IC_BASE + 0x0034))
#define rISTR3                 (*(volatile unsigned long *) (IC_BASE + 0x0038))
#define rIMSR3                 (*(volatile unsigned long *) (IC_BASE + 0x003C))
#define rIESR4                 (*(volatile unsigned long *) (IC_BASE + 0x0040))
#define rIECR4                 (*(volatile unsigned long *) (IC_BASE + 0x0044))
#define rISTR4                 (*(volatile unsigned long *) (IC_BASE + 0x0048))
#define rIMSR4                 (*(volatile unsigned long *) (IC_BASE + 0x004C))
#define rCIPSR0                (*(volatile unsigned long *) (IC_BASE + 0x0100))





























/* DMAC */
/* SYSREG */

#define SYSREG_BASE               0x10010000

#define GENERAL_CTRL_C2C          (SYSREG_BASE + 0x010C)
#define GENERAL_CTRL_GPS          (SYSREG_BASE + 0x0110)
#define ADC_CFG                   (SYSREG_BASE + 0x0118)
#define ISPBLK_CFG                (SYSREG_BASE + 0x020C)
#define LCDBLK_CFG                (SYSREG_BASE + 0x0210)
#define LCDBLK_CFG2               (SYSREG_BASE + 0x0214)
#define CAMBLK_CFG                (SYSREG_BASE + 0x0218)
#define USB_CFG                   (SYSREG_BASE + 0x021C)
#define PPMU_CON                  (SYSREG_BASE + 0x0320)
#define SMMU_CON                  (SYSREG_BASE + 0x0330)

/*~~~~~~~~~~~~~~~~~~~~~~~*/
#define rGENERAL_CTRL_C2C          (*(volatile unsigned long *) (SYSREG_BASE + 0x010C))
#define rGENERAL_CTRL_GPS          (*(volatile unsigned long *) (SYSREG_BASE + 0x0110))
#define rADC_CFG                   (*(volatile unsigned long *) (SYSREG_BASE + 0x0118))
#define rISPBLK_CFG                (*(volatile unsigned long *) (SYSREG_BASE + 0x020C))
#define rLCDBLK_CFG                (*(volatile unsigned long *) (SYSREG_BASE + 0x0210))
#define rLCDBLK_CFG2               (*(volatile unsigned long *) (SYSREG_BASE + 0x0214))
#define rCAMBLK_CFG                (*(volatile unsigned long *) (SYSREG_BASE + 0x0218))
#define rUSB_CFG                   (*(volatile unsigned long *) (SYSREG_BASE + 0x021C))
#define rPPMU_CON                  (*(volatile unsigned long *) (SYSREG_BASE + 0x0320))
#define rSMMU_CON                  (*(volatile unsigned long *) (SYSREG_BASE + 0x0330))





















/* TZPC */
/* TZASC */
/* SysMMU */
/* SROMC */
/* NAND Flash Controller */
/* EBI */
/* SD/MMC host controller */
/* PWM */

#define PWM_BASE              0x139D0000
#define PWM_ISP_BASE          0x12160000

#define TCFG0                 (PWM_BASE + 0x0000)
#define TCFG1                 (PWM_BASE + 0x0004)
#define TCON                  (PWM_BASE + 0x0008)
#define TCNTB0                (PWM_BASE + 0x000C)
#define TCMPB0                (PWM_BASE + 0x0010)
#define TCNTO0                (PWM_BASE + 0x0014)

/*~~~~~~~~~~*/
#define rTCFG0                 (*(volatile unsigned long *) (PWM_BASE + 0x0000))
#define rTCFG1                 (*(volatile unsigned long *) (PWM_BASE + 0x0004))
#define rTCON                  (*(volatile unsigned long *) (PWM_BASE + 0x0008))
#define rTCNTB0                (*(volatile unsigned long *) (PWM_BASE + 0x000C))
#define rTCMPB0                (*(volatile unsigned long *) (PWM_BASE + 0x0010))
#define rTCNTO0                (*(volatile unsigned long *) (PWM_BASE + 0x0014))




/* MCT */
/* WDT */
/* RTC */
/* UART */
/* I2C */
/* SPI */
/* TSI */
/* AudioSS */
/* IIS */


/* ADC */

#define ADC_BASE              0x126C0000

#define ADCCON                (ADC_BASE + 0x0000)
#define ADCDLY                (ADC_BASE + 0x0008)
#define ADCDAT                (ADC_BASE + 0x000C)
#define CLRINTADC             (ADC_BASE + 0x0018)
#define ADCMUX                (ADC_BASE + 0x001C)

/*~~~~~~~~~~~~~*/
#define rADCCON                (*(volatile unsigned long *) (ADC_BASE + 0x0000))
#define rADCDLY                (*(volatile unsigned long *) (ADC_BASE + 0x0008))
#define rADCDAT                (*(volatile unsigned long *) (ADC_BASE + 0x000C))
#define rCLRINTADC             (*(volatile unsigned long *) (ADC_BASE + 0x0018))
#define rADCMUX                (*(volatile unsigned long *) (ADC_BASE + 0x001C))












#endif
