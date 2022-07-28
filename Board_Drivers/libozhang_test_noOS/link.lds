SECTIONS
{
	/* 指定链接地址为 0x40000000, 需保证位置有关代码需运行在相应的内存地址上 */
	. = 0x40000000;
	/* 代码段(文本段)，存储代码编译后的二进制文本 */
	.text : {
			start.o
			* (.text)
	}
	/* 只读数据段（read only data） */
	.rodata ALIGN(4) : {
		*(.rodata*)
	}
  	/* 数据段 */
	.data ALIGN(4) : {
		*(.data)
	}
	/* bss段（Block Started by Symbol）未初始化的或者初始化为 0 的全局变量和静态变量，又叫 ZI 段（Zero Inital） */
	bss_start = .;
	.bss ALIGN(4): {
			* (.bss)
			* (COMMON)
	}
	bss_end = .;
}
