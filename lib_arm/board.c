/*
 * To match the U-Boot user interface on ARM platforms to the U-Boot
 * standard (as on PPC platforms), some messages with debug character
 * are removed from the default U-Boot build.
 *
 * Define DEBUG here if you want additional info as shown below
 * printed upon startup:
 *
 * U-Boot code: 00F00000 -> 00F3C774  BSS: -> 00FC3274
 * IRQ Stack: 00ebff7c
 * FIQ Stack: 00ebef7c
 */
#include <common.h>
#include <command.h>
#include <malloc.h>
#include <asm/io.h>
#include <movi.h>
#include <stdio_dev.h>
#include <timestamp.h>
#include <version.h>
#include "/home/sw/pp_self/1/iTop4412_uboot/include/net.h"
//#include <net.h>
#include <serial.h>
#include <nand.h>
#include <onenand_uboot.h>
#include <s5pc210.h>
#ifdef CONFIG_GENERIC_MMC
#include <mmc.h>
#endif
#undef DEBUG

#ifdef CONFIG_BITBANGMII
#include <miiphy.h>
#endif

#ifdef CONFIG_DRIVER_SMC91111
#include "../drivers/net/smc91111.h"
#endif
#ifdef CONFIG_DRIVER_LAN91C96
#include "../drivers/net/lan91c96.h"
#endif

#ifdef CONFIG_RECOVERY
extern int recovery_preboot(void);
#endif
DECLARE_GLOBAL_DATA_PTR;

ulong monitor_flash_len;

#ifdef CONFIG_HAS_DATAFLASH
extern int  AT91F_DataflashInit(void);
extern void dataflash_print_info(void);
#endif

#ifndef CONFIG_IDENT_STRING
#define CONFIG_IDENT_STRING ""
#endif

const char version_string[] =
	U_BOOT_VERSION" (" U_BOOT_DATE " - " U_BOOT_TIME ")"CONFIG_IDENT_STRING;

#ifdef CONFIG_DRIVER_RTL8019
extern void rtl8019_get_enetaddr (uchar * addr);
#endif

#if defined(CONFIG_HARD_I2C) || \
    defined(CONFIG_SOFT_I2C)
#include <i2c.h>
#endif


/************************************************************************
 * Coloured LED functionality
 ************************************************************************
 * May be supplied by boards if desired
 */
//[]sw2022-0501========
//void inline __coloured_LED_init (void) {}
void __coloured_LED_init (void) {}
void coloured_LED_init (void) __attribute__((weak, alias("__coloured_LED_init")));
//void inline __red_LED_on (void) {}
void __red_LED_on (void) {}
void red_LED_on (void) __attribute__((weak, alias("__red_LED_on")));
//void inline __red_LED_off(void) {}
void __red_LED_off(void) {}
void red_LED_off(void) __attribute__((weak, alias("__red_LED_off")));
//void inline __green_LED_on(void) {}
void __green_LED_on(void) {}
void green_LED_on(void) __attribute__((weak, alias("__green_LED_on")));
//void inline __green_LED_off(void) {}
void __green_LED_off(void) {}
void green_LED_off(void) __attribute__((weak, alias("__green_LED_off")));
//void inline __yellow_LED_on(void) {}
void __yellow_LED_on(void) {}
void yellow_LED_on(void) __attribute__((weak, alias("__yellow_LED_on")));
//void inline __yellow_LED_off(void) {}
void __yellow_LED_off(void) {}
void yellow_LED_off(void) __attribute__((weak, alias("__yellow_LED_off")));
//void inline __blue_LED_on(void) {}
void __blue_LED_on(void) {}
void blue_LED_on(void) __attribute__((weak, alias("__blue_LED_on")));
//void inline __blue_LED_off(void) {}
void __blue_LED_off(void) {}
void blue_LED_off(void) __attribute__((weak, alias("__blue_LED_off")));

/************************************************************************
 * Init Utilities							*
 ************************************************************************
 * Some of this code should be moved into the core functions,
 * or dropped completely,
 * but let's get it working (again) first...
 */

#if defined(CONFIG_ARM_DCC) && !defined(CONFIG_BAUDRATE)
#define CONFIG_BAUDRATE 115200
#endif
static int init_baudrate (void)
{
	char tmp[64];	/* long enough for environment variables */
	int i = getenv_r ("baudrate", tmp, sizeof (tmp));
	gd->bd->bi_baudrate = gd->baudrate = (i > 0)
			? (int) simple_strtoul (tmp, NULL, 10)
			: CONFIG_BAUDRATE;

	return (0);
}

static int off_charge(void)
{
	volatile int value = 0;
	int i,j;
	int val;

	/*---only evt use this---*/
	val = readl(GPX2PUD);
	val &= ~(0x3<<12); //dok,pullup/down disable
	writel(val,GPX2PUD);
	
	/*---volume key setting---*/
	__REG(GPL2CON) = __REG(GPL2CON)|0x1<<0;
	__REG(GPL2DAT) = __REG(GPL2DAT)|0x1<<0;//output 1

	__REG(GPX2CON) = __REG(GPX2CON)&~0xff; //gpx2.0,gpx2.1

	val = readl(GPX2PUD);
	val &= ~(0xf);
	val |= (0x5);
	writel(val,GPX2PUD);

	
	/*---gpx2.7 cok ---setting--*/
	val = readl(GPX2CON);
	val &= ~(0xf<<28);
	writel(val,GPX2CON);
	
	val = readl(GPX2PUD);
	val &= ~(0x3<<14); //cok gpx2.7,pullup/down disable
	writel(val,GPX2PUD);


	/*---this don't need ---*/
	val = readl(GPX1PUD);
	val &= ~(0x3<<10); //uok,pullup/down disable
	writel(val,GPX1PUD);
	

	/*---on key setting gpx0.2--*/
	val = readl(GPX0PUD);
	val &= ~(0x3<<4); //on_key,pullup/down disable
	writel(val,GPX0PUD);
	
	return 0;
}

static int display_banner (void)
{
	printf ("\n\n%s\n\n", version_string);
	debug ("U-Boot code: %08lX -> %08lX  BSS: -> %08lX\n",
	       _armboot_start, _bss_start, _bss_end);
#ifdef CONFIG_MODEM_SUPPORT
	debug ("Modem Support enabled\n");
#endif
#ifdef CONFIG_USE_IRQ
	debug ("IRQ Stack: %08lx\n", IRQ_STACK_START);
	debug ("FIQ Stack: %08lx\n", FIQ_STACK_START);
#endif

	return (0);
}

/*
 * WARNING: this code looks "cleaner" than the PowerPC version, but
 * has the disadvantage that you either get nothing, or everything.
 * On PowerPC, you might see "DRAM: " before the system hangs - which
 * gives a simple yet clear indication which part of the
 * initialization if failing.
 */
static int display_dram_config (void)
{
	int i;

#ifdef DEBUG
	puts ("RAM Configuration:\n");

	for(i=0; i<CONFIG_NR_DRAM_BANKS; i++) {
		printf ("Bank #%d: %08lx ", i, gd->bd->bi_dram[i].start);
		print_size (gd->bd->bi_dram[i].size, "\n");
	}
#else
	ulong size = 0;

	for (i=0; i<CONFIG_NR_DRAM_BANKS; i++) {
		size += gd->bd->bi_dram[i].size;
	}

#if  defined(CONFIG_SCP_1GDDR) ||  defined(CONFIG_SCP_2GDDR) || defined(CONFIG_SCP_1GDDR_Ubuntu) || defined(CONFIG_SCP_2GDDR_Ubuntu)  //add by dg

	size += 0x100000;
#endif

#if defined(CONFIG_POP_2GDDR) || defined(CONFIG_POP_2GDDR_Ubuntu) 
	puts("DRAM: ");
    puts("2G\n");
#else
  	puts("DRAM:	");
	print_size(size, "\n");
#endif
#endif

	return (0);
}

#ifndef CONFIG_SYS_NO_FLASH
static void display_flash_config (ulong size)
{
	puts ("Flash: ");
	print_size (size, "\n");
}
#endif /* CONFIG_SYS_NO_FLASH */

#if defined(CONFIG_HARD_I2C) || defined(CONFIG_SOFT_I2C)
static int init_func_i2c (void)
{
	puts ("I2C:   ");
	i2c_init (CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
	puts ("ready\n");
	return (0);
}
#endif

#if defined(CONFIG_CMD_PCI) || defined (CONFIG_PCI)
#include <pci.h>
static int arm_pci_init(void)
{
	pci_init();
	return 0;
}
#endif /* CONFIG_CMD_PCI || CONFIG_PCI */

/*
 * Breathe some life into the board...
 *
 * Initialize a serial port as console, and carry out some hardware
 * tests.
 *
 * The first part of initialization is running from Flash memory;
 * its main purpose is to initialize the RAM so that we
 * can relocate the monitor code to RAM.
 */

/*
 * All attempts to come up with a "common" initialization sequence
 * that works for all boards and architectures failed: some of the
 * requirements are just _too_ different. To get rid of the resulting
 * mess of board dependent #ifdef'ed code we now make the whole
 * initialization sequence configurable to the user.
 *
 * The requirements for any new initalization function is simple: it
 * receives a pointer to the "global data" structure as it's only
 * argument, and returns an integer return code, where 0 means
 * "continue" and != 0 means "fatal error, hang the system".
 */
typedef int (init_fnc_t) (void);

int print_cpuinfo (void);

init_fnc_t *init_sequence[] = {
#if defined(CONFIG_ARCH_CPU_INIT)
	arch_cpu_init,		/* basic arch cpu dependent setup */
#endif
	board_init,		/* basic board dependent setup */
//#if defined(CONFIG_USE_IRQ)
	interrupt_init,		/* set up exceptions */
//#endif
	//timer_init,		/* initialize timer */
#ifdef CONFIG_FSL_ESDHC
	//get_clocks,
#endif
	env_init,		/* initialize environment */
	init_baudrate,		/* initialze baudrate settings */
	serial_init,		/* serial communications setup */
	console_init_f,		/* stage 1 init of console */
	off_charge,		// xiebin.wang @ 20110531,for charger&power off device.

	display_banner,		/* say that we are here */
#if defined(CONFIG_DISPLAY_CPUINFO)
	print_cpuinfo,		/* display cpu info (and speed) */
#endif
#if defined(CONFIG_DISPLAY_BOARDINFO)
	checkboard,		/* display board info */
#endif
#if defined(CONFIG_HARD_I2C) || defined(CONFIG_SOFT_I2C)
	//init_func_i2c,
#endif
	dram_init,		/* configure available RAM banks */
#if defined(CONFIG_CMD_PCI) || defined (CONFIG_PCI)
	//arm_pci_init,
#endif
	display_dram_config,
	NULL,
};

void start_armboot (void)
{
//printf("target is ok ~~ in start_armboot");
	init_fnc_t **init_fnc_ptr;
	char *s;
	int mmc_exist = 0;
#if defined(CONFIG_VFD) || defined(CONFIG_LCD)
	unsigned long addr;
#endif

	/* Pointer is writable since we allocated a register for it */
	gd = (gd_t*)(_armboot_start - CONFIG_SYS_MALLOC_LEN - sizeof(gd_t));
	/* compiler optimization barrier needed for GCC >= 3.4 */
	__asm__ __volatile__("": : :"memory");

	memset ((void*)gd, 0, sizeof (gd_t));
	gd->bd = (bd_t*)((char*)gd - sizeof(bd_t));
	memset (gd->bd, 0, sizeof (bd_t));


//???????????????????????????????????????????????????????????????????????????
//	gd->flags |= GD_FLG_RELOC;


	//u-boot??????????????????
  monitor_flash_len = _bss_start - _armboot_start;

	for (init_fnc_ptr = init_sequence; *init_fnc_ptr; ++init_fnc_ptr) {
		if ((*init_fnc_ptr)() != 0) {
			hang ();
		}
	}
	
	/* armboot_start is defined in the board-specific linker script */
	mem_malloc_init (_armboot_start - CONFIG_SYS_MALLOC_LEN,
			CONFIG_SYS_MALLOC_LEN);

#ifndef CONFIG_SYS_NO_FLASH
	/* configure available FLASH banks */
	display_flash_config (flash_init ());
#endif /* CONFIG_SYS_NO_FLASH */

#ifdef CONFIG_VFD
#	ifndef PAGE_SIZE
#	  define PAGE_SIZE 4096
#	endif
	/*
	 * reserve memory for VFD display (always full pages)
	 */
	/* bss_end is defined in the board-specific linker script */
	addr = (_bss_end + (PAGE_SIZE - 1)) & ~(PAGE_SIZE - 1);
	vfd_setmem (addr);
	gd->fb_base = addr;
#endif /* CONFIG_VFD */

#ifdef CONFIG_LCD
	/* board init may have inited fb_base */
	if (!gd->fb_base) {
#		ifndef PAGE_SIZE
#		  define PAGE_SIZE 4096
#		endif
		/*
		 * reserve memory for LCD display (always full pages)
		 */
		/* bss_end is defined in the board-specific linker script */
		addr = (_bss_end + (PAGE_SIZE - 1)) & ~(PAGE_SIZE - 1);
		lcd_setmem (addr);
		gd->fb_base = addr;
	}
#endif /* CONFIG_LCD */

#if defined(CONFIG_CMD_NAND)
	puts ("NAND:	");
	nand_init();		/* go init the NAND */
#endif

#if defined(CONFIG_CMD_ONENAND)
	onenand_init();
#endif

#ifdef CONFIG_GENERIC_MMC
	puts ("MMC:   ");
	mmc_exist = mmc_initialize (gd->bd);
	if (mmc_exist != 0)
	{
		puts ("0 MB\n");
	}

#endif

	
#ifdef CONFIG_HAS_DATAFLASH
	AT91F_DataflashInit();
	dataflash_print_info();
#endif

	/* initialize environment */
	env_relocate ();

#ifdef CONFIG_VFD
	/* must do this after the framebuffer is allocated */
	drv_vfd_init();
#endif /* CONFIG_VFD */

#ifdef CONFIG_SERIAL_MULTI
	serial_initialize();
#endif

	/* IP Address */
	gd->bd->bi_ip_addr = getenv_IPaddr ("ipaddr");

	stdio_init ();	/* get the devices list going. */

	jumptable_init ();

#if defined(CONFIG_API)
	/* Initialize API */
	api_init ();
#endif

	console_init_r ();	/* fully init console as a device */

#if defined(CONFIG_ARCH_MISC_INIT)
	/* miscellaneous arch dependent initialisations */
	arch_misc_init ();
#endif
#if defined(CONFIG_MISC_INIT_R)
	/* miscellaneous platform dependent initialisations */
	misc_init_r ();
#endif

	/* enable exceptions */
	enable_interrupts ();

	/* Perform network card initialisation if necessary */
#ifdef CONFIG_DRIVER_TI_EMAC
	/* XXX: this needs to be moved to board init */
extern void davinci_eth_set_mac_addr (const u_int8_t *addr);
	if (getenv ("ethaddr")) {
		uchar enetaddr[6];
		eth_getenv_enetaddr("ethaddr", enetaddr);
		davinci_eth_set_mac_addr(enetaddr);
	}
#endif

#if defined(CONFIG_DRIVER_SMC91111) || defined (CONFIG_DRIVER_LAN91C96)
	/* XXX: this needs to be moved to board init */
	if (getenv ("ethaddr")) {
		uchar enetaddr[6];
		eth_getenv_enetaddr("ethaddr", enetaddr);
		smc_set_mac_addr(enetaddr);
	}
#endif /* CONFIG_DRIVER_SMC91111 || CONFIG_DRIVER_LAN91C96 */

	/* Initialize from environment */
	if ((s = getenv ("loadaddr")) != NULL) {
		load_addr = simple_strtoul (s, NULL, 16);
	}
#if defined(CONFIG_CMD_NET)


//void copy_filename (char *dst, char *src, int size)
//{
//  if (*src && (*src == '"')) {
//    ++src;
//    --size;
//  }
//  while ((--size > 0) && *src && (*src != '"')) {
//    *dst++ = *src++;
//  }
//  *dst = '\0';
//}
	if ((s = getenv ("Bootfile")) != NULL) {
		copy_filename (BootFile, s, sizeof (BootFile));
	}
#endif

#ifdef BOARD_LATE_INIT
	board_late_init ();
#endif

#ifdef CONFIG_BITBANGMII
	bb_miiphy_init();
#endif
//  printf("CONFIG_CMD_NET = CONFIG_CMD_NET +++++");
#if defined(CONFIG_CMD_NET)

#if defined(CONFIG_NET_MULTI)
puts ("Net:   ");
#endif

extern int eth_initialize(bd_t *bis);
eth_initialize(gd->bd);
/* net/libnet.a  DM9621ANP 
 * -->hub??????usb4640 USB3503A
 * --> ??????usb4640??????HSIC??????
 * ???XhsicSTROBE0???XhsicDATA0????????????exynos4412???*/

#if defined(CONFIG_RESET_PHY_R)
debug ("Reset Ethernet PHY\n");
reset_phy();
#endif

#endif

/*
	char tmp_cmd[100];	
	sprintf(tmp_cmd, "emmc open 0");
	run_command(tmp_cmd, 0);
*/				
	/* main_loop() can return to retry autoboot, if so just run it again. */
#ifdef CONFIG_RECOVERY //mj for factory reset
	recovery_preboot();
#endif

	for (;;) {
		main_loop ();
	}

	/* NOTREACHED - no way out of command loop except booting */
}

void hang (void)
{
	puts ("### ERROR ### Please RESET the board ###\n");
	for (;;);
}
