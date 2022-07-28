#include "s3c4412_uart.h"
#include "s3c4412_gpio.h"

void uart_init(void)
{
	//GPA0CON = 0x22222222; // 设置 GPA0CON 为 UART0 和 UART1 模式
	GPA1CON = 0x222222; // 设置 GPA1CON 为 UART2 和 UART3 模式

	ULCON2 = 0x00000003; // 8bit数据域 1bit 停止位，无奇偶校验位
	UCON2 = 0x5; // 设置收发均为 poll mode
	UFCON2 = 0x111; // 设置 FIFO 及相应的触发级别
	UMCON2 = 0x00000000; // 禁止 AFC，禁止Modem
	/* 波特率设置为 115200 */
	/* 	DIV_VAL = UBRDIVn + UFRACVALn/16
	   	DIV_VAL = (SCLK_UART/(bps * 16)) - 1
	   	Where, the divisor should be from 1 to (216 – 1).
       	By using UFRACVALn, you can generate the Baud rate more accurately.
       	For example, if the Baud rate is 115200 bps and SCLK_UART is 100 MHz, UBRDIVn and UFRACVALn are:
	   	DIV_VAL = (100000000/(115200 * 16)) – 1 = 13.02 – 1 = 12.02
	    UBRDIVn = 12 (integer part of DIV_VAL)
		UFRACVALn/16 = 0.02
		Therefore, UFRACVALn = 0 */
	UBRDIV2 = 53;
	UFRACVAL2 = 4;

	return;
} 

void putc(unsigned char c)
{
	while (!(UTRSTAT2 & (1 << 2)));
	UTXH2 = c;
	
	return;
}

unsigned char getc(void)
{
	char c = '\0';
	while (!(UTRSTAT2 & (1 << 0)));
	c = URXH2;
	
	return c;
}