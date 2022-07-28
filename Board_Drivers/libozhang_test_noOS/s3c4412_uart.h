#ifndef S3C4412_UART_H
#define S3C4412_UART_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * UART Register
 */

#define UART0BASE 0x13800000
#define UART1BASE 0x13810000
#define UART2BASE 0x13820000
#define UART3BASE 0x13830000

#define ULCON0 (*(volatile unsigned long *)(UART0BASE + 0x0000))
#define UCON0 (*(volatile unsigned long *)(UART0BASE + 0x0004))
#define UFCON0 (*(volatile unsigned long *)(UART0BASE + 0x0008))
#define UMCON0 (*(volatile unsigned long *)(UART0BASE + 0x000C))
#define UTRSTAT0 (*(volatile unsigned long *)(UART0BASE + 0x0010))
#define UERSTAT0 (*(volatile unsigned long *)(UART0BASE + 0x0014))
#define UFSTAT0 (*(volatile unsigned long *)(UART0BASE + 0x0018))
#define UMSTAT0 (*(volatile unsigned long *)(UART0BASE + 0x001C))
#define UTXH0 (*(volatile unsigned long *)(UART0BASE + 0x0020))
#define URXH0 (*(volatile unsigned long *)(UART0BASE + 0x0024))
#define UBRDIV0 (*(volatile unsigned long *)(UART0BASE + 0x0028))
#define UFRACVAL0 (*(volatile unsigned long *)(UART0BASE + 0x002C))
#define UINTP0 (*(volatile unsigned long *)(UART0BASE + 0x0030))
#define UINTSP0 (*(volatile unsigned long *)(UART0BASE + 0x0034))
#define UINTM0 (*(volatile unsigned long *)(UART0BASE + 0x0038))

#define ULCON1 (*(volatile unsigned long *)(UART1BASE + 0x0000))
#define UCON1 (*(volatile unsigned long *)(UART1BASE + 0x0004))
#define UFCON1 (*(volatile unsigned long *)(UART1BASE + 0x0008))
#define UMCON1 (*(volatile unsigned long *)(UART1BASE + 0x000C))
#define UTRSTAT1 (*(volatile unsigned long *)(UART1BASE + 0x0010))
#define UERSTAT1 (*(volatile unsigned long *)(UART1BASE + 0x0014))
#define UFSTAT1 (*(volatile unsigned long *)(UART1BASE + 0x0018))
#define UMSTAT1 (*(volatile unsigned long *)(UART1BASE + 0x001C))
#define UTXH1 (*(volatile unsigned long *)(UART1BASE + 0x0020))
#define URXH1 (*(volatile unsigned long *)(UART1BASE + 0x0024))
#define UBRDIV1 (*(volatile unsigned long *)(UART1BASE + 0x0028))
#define UFRACVAL1 (*(volatile unsigned long *)(UART1BASE + 0x002C))
#define UINTP1 (*(volatile unsigned long *)(UART1BASE + 0x0030))
#define UINTSP1 (*(volatile unsigned long *)(UART1BASE + 0x0034))
#define UINTM1 (*(volatile unsigned long *)(UART1BASE + 0x0038))

#define ULCON2 (*(volatile unsigned long *)(UART2BASE + 0x0000))
#define UCON2 (*(volatile unsigned long *)(UART2BASE + 0x0004))
#define UFCON2 (*(volatile unsigned long *)(UART2BASE + 0x0008))
#define UMCON2 (*(volatile unsigned long *)(UART2BASE + 0x000C))
#define UTRSTAT2 (*(volatile unsigned long *)(UART2BASE + 0x0010))
#define UERSTAT2 (*(volatile unsigned long *)(UART2BASE + 0x0014))
#define UFSTAT2 (*(volatile unsigned long *)(UART2BASE + 0x0018))
#define UMSTAT2 (*(volatile unsigned long *)(UART2BASE + 0x001C))
#define UTXH2 (*(volatile unsigned long *)(UART2BASE + 0x0020))
#define URXH2 (*(volatile unsigned long *)(UART2BASE + 0x0024))
#define UBRDIV2 (*(volatile unsigned long *)(UART2BASE + 0x0028))
#define UFRACVAL2 (*(volatile unsigned long *)(UART2BASE + 0x002C))
#define UINTP2 (*(volatile unsigned long *)(UART2BASE + 0x0030))
#define UINTSP2 (*(volatile unsigned long *)(UART2BASE + 0x0034))
#define UINTM2 (*(volatile unsigned long *)(UART2BASE + 0x0038))

#define ULCON3 (*(volatile unsigned long *)(UART3BASE + 0x0000))
#define UCON3 (*(volatile unsigned long *)(UART3BASE + 0x0004))
#define UFCON3 (*(volatile unsigned long *)(UART3BASE + 0x0008))
#define UMCON3 (*(volatile unsigned long *)(UART3BASE + 0x000C))
#define UTRSTAT3 (*(volatile unsigned long *)(UART3BASE + 0x0010))
#define UERSTAT3 (*(volatile unsigned long *)(UART3BASE + 0x0014))
#define UFSTAT3 (*(volatile unsigned long *)(UART3BASE + 0x0018))
#define UMSTAT3 (*(volatile unsigned long *)(UART3BASE + 0x001C))
#define UTXH3 (*(volatile unsigned long *)(UART3BASE + 0x0020))
#define URXH3 (*(volatile unsigned long *)(UART3BASE + 0x0024))
#define UBRDIV3 (*(volatile unsigned long *)(UART3BASE + 0x0028))
#define UFRACVAL3 (*(volatile unsigned long *)(UART3BASE + 0x002C))
#define UINTP3 (*(volatile unsigned long *)(UART3BASE + 0x0030))
#define UINTSP3 (*(volatile unsigned long *)(UART3BASE + 0x0034))
#define UINTM3 (*(volatile unsigned long *)(UART3BASE + 0x0038))

#ifdef __cplusplus
}
#endif

#endif