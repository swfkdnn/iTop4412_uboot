#include <asm/byteorder.h>
#include <common.h>
#include <command.h>
#include <fastboot.h>
#if defined(CFG_FASTBOOT_SDMMCBSP)
#include <mmc.h>
#endif
#include <decompress_ext4.h>

//#ifndef SMDK4212_ID
//#define SMDK4212_ID 0x43220000
//#endif
//#ifndef SMDK4412_ID
//#define SMDK4412_ID 0xE4412000
//#endif


extern unsigned int OmPin;
extern unsigned int s5pc210_cpu_id;
extern int Is_TC4_Dvt;
int dev_number_write =0;
int fuse_by_fastboot = 0;

#if defined(CONFIG_FASTBOOT)

/* Use do_reset for fastboot's 'reboot' command */
extern int do_reset (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
/* Use do_fat_fsload for direct image fusing from sd card */
extern int do_fat_fsload (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
/* Use do_setenv and do_saveenv to permenantly save data */
int do_saveenv (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
int do_setenv ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
/* Use do_bootm and do_go for fastboot's 'boot' command */
int do_bootm (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
int do_go (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);

#if defined(CFG_FASTBOOT_ONENANDBSP)
#define CFG_FASTBOOT_FLASHCMD			do_onenand
/* Use do_onenand for fastboot's flash commands */
extern int do_onenand(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[]);
#elif defined(CFG_FASTBOOT_NANDBSP)
#define CFG_FASTBOOT_FLASHCMD			do_nand
/* Use do_nand for fastboot's flash commands */
extern int do_nand(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[]);
#endif

#if defined(CFG_FASTBOOT_SDMMCBSP)
int do_movi(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[]);
int do_mmcops(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[]);
int do_mmcops_secure(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[]);
int get_mmc_part_info(char *device_name, int part_num, int *start, int *count, unsigned char *pid);
struct mmc *find_mmc_device(int dev_num);
#endif

/* LCD */
void LCD_turnon(void) {}
void LCD_setfgcolor(unsigned int color){}
void LCD_setleftcolor(unsigned int color){}
void LCD_setprogress(int percentage){}

/* Forward decl */
static int rx_handler (const unsigned char *buffer, unsigned int buffer_size);
static void reset_handler (void);

/* cmd_fastboot_interface	in fastboot.h	*/
static struct cmd_fastboot_interface interface =
{
	.rx_handler            = rx_handler,
	.reset_handler         = reset_handler,
	.product_name          = NULL,
	.serial_no             = NULL,
	.nand_block_size       = 0,
	.transfer_buffer       = (unsigned char *)0xffffffff,
	.transfer_buffer_size  = 0,
};

static unsigned int download_size;
static unsigned int download_bytes;
//static unsigned int download_bytes_unpadded;
static unsigned int download_error;

/* To support the Android-style naming of flash */
#define MAX_PTN 16
static fastboot_ptentry ptable[MAX_PTN];
static unsigned int pcount;
static int static_pcount = -1;

/* Default partition table (see cpu/.../fastboot.c) */
extern fastboot_ptentry *ptable_default;
extern unsigned int ptable_default_size;

static void set_env(char *var, char *val)
{
	char *setenv[4]  = { "setenv", NULL, NULL, NULL, };

	setenv[1] = var;
	setenv[2] = val;

	do_setenv(NULL, 0, 3, setenv);
}

static void save_env(struct fastboot_ptentry *ptn,
		     char *var, char *val)
{
#ifndef CFG_FASTBOOT_SDMMCBSP
	char start[32], length[32];
	char ecc_type[32];

	char *lock[5]    = { "nand", "lock",   NULL, NULL, NULL, };
	char *unlock[5]  = { "nand", "unlock", NULL, NULL, NULL, };
	char *ecc[4]     = { "nand", "ecc",    NULL, NULL, };
	char *saveenv[2] = { "setenv", NULL, };

	lock[2] = unlock[2] = start;
	lock[3] = unlock[3] = length;

	set_env (var, val);

	/* Some flashing requires the nand's ecc to be set */
	ecc[2] = ecc_type;
	if ((ptn->flags & FASTBOOT_PTENTRY_FLAGS_WRITE_HW_ECC) &&
	    (ptn->flags & FASTBOOT_PTENTRY_FLAGS_WRITE_SW_ECC))	{
		/* Both can not be true */
		printf("Warning can not do hw and sw ecc for partition '%s'\n", ptn->name);
		printf("Ignoring these flags\n");
	} else if (ptn->flags & FASTBOOT_PTENTRY_FLAGS_WRITE_HW_ECC) {
		sprintf(ecc_type, "hw");
		CFG_FASTBOOT_FLASHCMD(NULL, 0, 3, ecc);
	} else if (ptn->flags & FASTBOOT_PTENTRY_FLAGS_WRITE_SW_ECC) {
		sprintf(ecc_type, "sw");
		CFG_FASTBOOT_FLASHCMD(NULL, 0, 3, ecc);
	}
	sprintf(start, "0x%x", ptn->start);
	sprintf(length, "0x%x", ptn->length);

	/* This could be a problem is there is an outstanding lock */
	CFG_FASTBOOT_FLASHCMD(NULL, 0, 4, unlock);
	do_saveenv(NULL, 0, 1, saveenv);
	CFG_FASTBOOT_FLASHCMD(NULL, 0, 4, lock);
#endif
}

static void reset_handler ()
{
	/* If there was a download going on, bail */
	download_size = 0;
	download_bytes = 0;
	//download_bytes_unpadded = 0;
	download_error = 0;
}


/* When save = 0, just parse.  The input is unchanged
   When save = 1, parse and do the save.  The input is changed */
static int parse_env(void *ptn, char *err_string, int save, int debug)
{
	int ret = 1;
	unsigned int sets = 0;
	unsigned int comment_start = 0;
	char *var = NULL;
	char *var_end = NULL;
	char *val = NULL;
	char *val_end = NULL;
	unsigned int i;

	char *buff = (char *)interface.transfer_buffer;
	//unsigned int size = download_bytes_unpadded;
	unsigned int size = download_bytes;

	/* The input does not have to be null terminated.
	   This will cause a problem in the corner case
	   where the last line does not have a new line.
	   Put a null after the end of the input.

	   WARNING : Input buffer is assumed to be bigger
	   than the size of the input */
	if (save)
		buff[size] = 0;

	for (i = 0; i < size; i++) {

		if (NULL == var) {

			/*
			 * Check for comments, comment ok only on
			 * mostly empty lines
			 */
			if (buff[i] == '#')
				comment_start = 1;

			if (comment_start) {
				if  ((buff[i] == '\r') ||
				     (buff[i] == '\n')) {
					comment_start = 0;
				}
			} else {
				if (!((buff[i] == ' ') ||
				      (buff[i] == '\t') ||
				      (buff[i] == '\r') ||
				      (buff[i] == '\n'))) {
					/*
					 * Normal whitespace before the
					 * variable
					 */
					var = &buff[i];
				}
			}

		} else if (((NULL == var_end) || (NULL == val)) &&
			   ((buff[i] == '\r') || (buff[i] == '\n'))) {

			/* This is the case when a variable
			   is unset. */

			if (save) {
				/* Set the var end to null so the
				   normal string routines will work

				   WARNING : This changes the input */
				buff[i] = '\0';

				save_env(ptn, var, val);

				if (debug)
					printf("Unsetting %s\n", var);
			}

			/* Clear the variable so state is parse is back
			   to initial. */
			var = NULL;
			var_end = NULL;
			sets++;
		} else if (NULL == var_end) {
			if ((buff[i] == ' ') ||
			    (buff[i] == '\t'))
				var_end = &buff[i];
		} else if (NULL == val) {
			if (!((buff[i] == ' ') ||
			      (buff[i] == '\t')))
				val = &buff[i];
		} else if (NULL == val_end) {
			if ((buff[i] == '\r') ||
			    (buff[i] == '\n')) {
				/* look for escaped cr or ln */
				if ('\\' == buff[i - 1]) {
					/* check for dos */
					if ((buff[i] == '\r') &&
					    (buff[i+1] == '\n'))
						buff[i + 1] = ' ';
					buff[i - 1] = buff[i] = ' ';
				} else {
					val_end = &buff[i];
				}
			}
		} else {
			sprintf(err_string, "Internal Error");

			if (debug)
				printf("Internal error at %s %d\n",
				       __FILE__, __LINE__);
			return 1;
		}
		/* Check if a var / val pair is ready */
		if (NULL != val_end) {
			if (save) {
				/* Set the end's with nulls so
				   normal string routines will
				   work.

				   WARNING : This changes the input */
				*var_end = '\0';
				*val_end = '\0';

				save_env(ptn, var, val);

				if (debug)
					printf("Setting %s %s\n", var, val);
			}

			/* Clear the variable so state is parse is back
			   to initial. */
			var = NULL;
			var_end = NULL;
			val = NULL;
			val_end = NULL;

			sets++;
		}
	}

	/* Corner case
	   Check for the case that no newline at end of the input */
	if ((NULL != var) &&
	    (NULL == val_end)) {
		if (save) {
			/* case of val / val pair */
			if (var_end)
				*var_end = '\0';
			/* else case handled by setting 0 past
			   the end of buffer.
			   Similar for val_end being null */
			save_env(ptn, var, val);

			if (debug) {
				if (var_end)
					printf("Trailing Setting %s %s\n", var, val);
				else
					printf("Trailing Unsetting %s\n", var);
			}
		}
		sets++;
	}
	/* Did we set anything ? */
	if (0 == sets)
		sprintf(err_string, "No variables set");
	else
		ret = 0;

	return ret;
}

static int saveenv_to_ptn(struct fastboot_ptentry *ptn, char *err_string)
{
	int ret = 1;
	int save = 0;
	int debug = 0;

	/* err_string is only 32 bytes
	   Initialize with a generic error message. */
	sprintf(err_string, "%s", "Unknown Error");

	/* Parse the input twice.
	   Only save to the enviroment if the entire input if correct */
	save = 0;
	if (0 == parse_env(ptn, err_string, save, debug)) {
		save = 1;
		ret = parse_env(ptn, err_string, save, debug);
	}
	return ret;
}

#if defined(CFG_FASTBOOT_ONENANDBSP) || defined(CFG_FASTBOOT_NANDBSP)
static int write_to_ptn(struct fastboot_ptentry *ptn, unsigned int addr, unsigned int size)
{
	int ret = 1;
	char start[32], length[32];
	char wstart[32], wlength[32], wbuffer[32];
	char write_type[32];

	/* do_nand and do_onenand do not check argv[0] */
	char *argv_erase[5]  = { NULL, "erase",  NULL, NULL, NULL, };
	char *argv_write[6]  = { NULL, NULL,  NULL, NULL, NULL, NULL, };

	argv_erase[2] = start;
	argv_erase[3] = length;

	argv_write[1] = write_type;
	argv_write[2] = wbuffer;
	argv_write[3] = wstart;
	argv_write[4] = wlength;

	printf("flashing '%s'\n", ptn->name);

	sprintf(start, "0x%x", ptn->start);
	if (ptn->length == 0)
	{
		CFG_FASTBOOT_FLASHCMD(NULL, 0, 3, argv_erase);
	}
	else
	{
		sprintf(length, "0x%x", ptn->length);
		CFG_FASTBOOT_FLASHCMD(NULL, 0, 4, argv_erase);
	}

	/* Which flavor of write to use */
	if (ptn->flags & FASTBOOT_PTENTRY_FLAGS_WRITE_YAFFS)
	{
		sprintf(write_type, "write.yaffs");
		sprintf(wlength, "0x%x", size);
	}
	else
	{
		sprintf(write_type, "write");
		if (interface.nand_block_size &&
			(size % interface.nand_block_size))
		{
			size = (size + interface.nand_block_size - 1)
					/ interface.nand_block_size * interface.nand_block_size;
			size += 0x20000;
		}
		sprintf(wlength, "0x%x", size);
	}
	sprintf(wbuffer, "0x%x", addr);
	sprintf(wstart, "0x%x", ptn->start);


	ret = CFG_FASTBOOT_FLASHCMD(NULL, 0, 5, argv_write);

#if 0
	if (0 == repeat) {
		if (ret) /* failed */
			save_block_values(ptn, 0, 0);
		else     /* success */
			save_block_values(ptn, ptn->start, download_bytes);
	}
#endif

	return ret;
}
#endif

#if defined(CFG_FASTBOOT_SDMMCBSP)
static int write_to_ptn_sdmmc(struct fastboot_ptentry *ptn, unsigned int addr, unsigned int size)
{
	int ret = 1;
	char device[32], part[32];
	char start[32], length[32], buffer[32];

	char *argv[6]  = { NULL, "write", NULL, NULL, NULL, NULL, };
	int argc = 0;

	if ((ptn->length != 0) && (size > ptn->length))
	{
		printf("Error: Image size is larger than partition size!\n");
		return 1;
	}

	printf("flashing '%s'\n", ptn->name);

	if (ptn->flags & FASTBOOT_PTENTRY_FLAGS_USE_MMC_CMD)
	{
		argv[2] = device;
		argv[3] = buffer;
		argv[4] = start;
		argv[5] = length;

		sprintf(device, "mmc %d", 1);
		sprintf(buffer, "0x%x", addr);
		sprintf(start, "0x%x", (ptn->start / CFG_FASTBOOT_SDMMC_BLOCKSIZE));
		sprintf(length, "0x%x", (ptn->length / CFG_FASTBOOT_SDMMC_BLOCKSIZE));
#if 0
#if defined(CONFIG_SECURE)
		ret = do_mmcops_secure(NULL, 0, 6, argv);
#else
		ret = do_mmcops(NULL, 0, 6, argv);
#endif
#endif
		if (check_compress_ext4((char*)addr,ptn->length) != 0) {
			ret = do_mmcops(NULL, 0, 6, argv);
		} else {
			printf("Compressed ext4 image\n");
			ret = write_compressed_ext4((char*)addr,
					ptn->start / CFG_FASTBOOT_SDMMC_BLOCKSIZE);
		}
	}
	else if (ptn->flags & FASTBOOT_PTENTRY_FLAGS_USE_MOVI_CMD)
	{
		argv[2] = part;
		argv[3] = buffer;

		argc = 4;

		/* use the partition name that can be understood by a command, movi */
		if (!strcmp(ptn->name, "bootloader"))
		{//ly
			if ( dev_number_write == 0)
				strncpy(part, "emmc-uboot", 11);
			else
				strncpy(part, "u-boot", 7);
		}
		else if (!strcmp(ptn->name, "ramdisk"))
		{
			strncpy(part, "rootfs", 7);
			argv[4] = length;
			sprintf(length, "0x%x",
				((size + CFG_FASTBOOT_SDMMC_BLOCKSIZE - 1)
				/ CFG_FASTBOOT_SDMMC_BLOCKSIZE ) * CFG_FASTBOOT_SDMMC_BLOCKSIZE);
			argc++;
		}
		#ifdef CONFIG_RECOVERY
		else if (!strcmp(ptn->name, "Recovery"))
		{
			strncpy(part, "Recovery", 7);
			argv[4] = length;
			sprintf(length, "0x%x",
				((size + CFG_FASTBOOT_SDMMC_BLOCKSIZE - 1)
				/ CFG_FASTBOOT_SDMMC_BLOCKSIZE ) * CFG_FASTBOOT_SDMMC_BLOCKSIZE);
			argc++;
		}
		#endif

		else	/* kernel, fwbl1 */
		{
			argv[2] = ptn->name;
		}
		sprintf(buffer, "0x%x", addr);

		ret = do_movi(NULL, 0, argc, argv);

		/* the return value of do_movi is different from usual commands. Hence the followings. */
		ret = 1 - ret;
	}

	return ret;
}
#endif


static int rx_handler (const unsigned char *buffer, unsigned int buffer_size)
{
  printf("[%s %s] %s: %s: %d\n", \
      __DATE__, __TIME__, __FILE__, __func__, __LINE__);
	int ret = 1;

  printf("[%s %s] %s: %s: %d\n", \
            __DATE__, __TIME__, __FILE__, __func__, __LINE__);
	/* Use 65 instead of 64
	   null gets dropped
	   strcpy's need the extra byte */
	char response[65];

	if (download_size)
	{
		/* Something to download */

		if (buffer_size)
		{
			/* Handle possible overflow */
			unsigned int transfer_size = download_size - download_bytes;

			if (buffer_size < transfer_size)
				transfer_size = buffer_size;

			/* Save the data to the transfer buffer */
			memcpy (interface.transfer_buffer + download_bytes,
				buffer, transfer_size);

			download_bytes += transfer_size;

			/* Check if transfer is done */
			if (download_bytes >= download_size)
			{
				/* Reset global transfer variable,
				   Keep download_bytes because it will be
				   used in the next possible flashing command */
				download_size = 0;

				if (download_error)
				{
					/* There was an earlier error */
					sprintf(response, "ERROR");
				}
				else
				{
					/* Everything has transferred,
					   send the OK response */
					sprintf(response, "OKAY");
				}
				fastboot_tx_status(response, strlen(response), FASTBOOT_TX_ASYNC);

				printf ("\ndownloading of %d bytes finished\n", download_bytes);
				LCD_setprogress(0);
			}

			/* Provide some feedback */
			if (download_bytes && download_size &&
			    0 == (download_bytes & (0x100000 - 1)))
			{
				/* Some feeback that the download is happening */
				if (download_error)
					printf("X");
				else
					printf(".");
				if (0 == (download_bytes %
					  (80 * 0x100000)))
					printf("\n");

				LCD_setfgcolor(0x2E8B57);
				LCD_setprogress(download_bytes / (download_size/100));
			}
		}
		else
		{
			/* Ignore empty buffers */
			printf ("Warning empty download buffer\n");
			printf ("Ignoring\n");
		}
		ret = 0;
	}
	else
	{
		/* A command */

		/* Cast to make compiler happy with string functions */
		const char *cmdbuf = (char *) buffer;

		/* Generic failed response */
		sprintf(response, "FAIL");

		/* reboot
		   Reboot the board. */
		if (memcmp(cmdbuf, "reboot", 6) == 0)
		{
			if (!strcmp(cmdbuf + 6, "-bootloader"))
			{
				strcpy((char *)interface.transfer_buffer, (char *)FASTBOOT_REBOOT_MAGIC);
			}
			else
			{
				memset(interface.transfer_buffer, 0x0, FASTBOOT_REBOOT_MAGIC_SIZE);
			}

			sprintf(response,"OKAY");
			fastboot_tx_status(response, strlen(response), FASTBOOT_TX_SYNC);
			//udelay (1000000); /* 1 sec */

			do_reset (NULL, 0, 0, NULL);

			/* This code is unreachable,
			   leave it to make the compiler happy */
			return 0;
		}

		/* getvar
		   Get common fastboot variables
		   Board has a chance to handle other variables */
		if (memcmp(cmdbuf, "getvar:", 7) == 0)
		{
			strcpy(response,"OKAY");

			if (!strcmp(cmdbuf + 7, "version"))
			{
				strcpy(response + 4, FASTBOOT_VERSION);
			}
			else if (!strcmp(cmdbuf + 7, "product"))
			{
				if (interface.product_name)
					strcpy(response + 4, interface.product_name);
			}
			else if (!strcmp(cmdbuf + 7, "serialno"))
			{
				if (interface.serial_no)
					strcpy(response + 4, interface.serial_no);
			}
			else if (!strcmp(cmdbuf + 7, "downloadsize"))
			{
				if (interface.transfer_buffer_size)
					sprintf(response + 4, "%08x", interface.transfer_buffer_size);
			}
			else
			{
				fastboot_getvar(cmdbuf + 7, response + 4);
			}
			ret = 0;
			goto send_tx_status;
		}

		/* erase
		   Erase a register flash partition
		   Board has to set up flash partitions */
		if (memcmp(cmdbuf, "erase:", 6) == 0)
		{
			struct fastboot_ptentry *ptn;

			ptn = fastboot_flash_find_ptn(cmdbuf + 6);
			if (ptn == 0)
			{
				sprintf(response, "FAILpartition does not exist");
				ret = 0;
				goto send_tx_status;
			}

			char start[32], length[32];
			int status;

			if (1 == dev_number_write || OmPin == BOOT_MMCSD) {
			printf("erasing(formatting) '%s'\n", ptn->name);
			} else if (OmPin == BOOT_EMMC441 || OmPin == BOOT_EMMC43) {
				printf("erasing '%s'\n", ptn->name);
			} else if (OmPin == BOOT_ONENAND) {
			printf("erasing '%s'\n", ptn->name);
				}
			
			LCD_setfgcolor(0x7FFFD4);
			LCD_setprogress(100);


			if (1 == dev_number_write) {
				// Temporary (but, simplest) implementation
				char run_cmd[80];
				status = 1;
				if (!strcmp(ptn->name, "userdata"))
				{
					sprintf(run_cmd, "ext3format mmc 1:3");
					status = run_command(run_cmd, 0);
				}
				else if (!strcmp(ptn->name, "cache"))
				{
					sprintf(run_cmd, "ext3format mmc 1:4");
					status = run_command(run_cmd, 0);
				}
				else if (!strcmp(ptn->name, "fat"))
				{
					sprintf(run_cmd, "fatformat mmc 1:1");
					status = run_command(run_cmd, 0);
				}
			} else if (OmPin == BOOT_MMCSD) {
				// Temporary (but, simplest) implementation
				char run_cmd[80];
				status = 1;
				if (!strcmp(ptn->name, "userdata"))
				{
					sprintf(run_cmd, "ext3format mmc 0:3");
					status = run_command(run_cmd, 0);
				}
				else if (!strcmp(ptn->name, "cache"))
				{
					sprintf(run_cmd, "ext3format mmc 0:4");
					status = run_command(run_cmd, 0);
				}
				else if (!strcmp(ptn->name, "fat"))
				{
					sprintf(run_cmd, "fatformat mmc 0:1");
					status = run_command(run_cmd, 0);
				}
			}else if(OmPin == BOOT_EMMC441 || OmPin == BOOT_EMMC43) {
				char run_cmd[80];
				status = 1;
				if (!strcmp(ptn->name, "userdata")) {
					sprintf(run_cmd, "ext3format mmc 0:3");
					status = run_command(run_cmd, 0);
				} else if (!strcmp(ptn->name, "cache")) {
					sprintf(run_cmd, "ext3format mmc 0:4");
					status = run_command(run_cmd, 0);
				} else if (!strcmp(ptn->name, "fat")) {
					sprintf(run_cmd, "fatformat mmc 0:1");
					status = run_command(run_cmd, 0);
				}
			}
//#else
			else if(OmPin == BOOT_ONENAND) {
#if defined(CFG_FASTBOOT_ONENANDBSP)
				int argc_erase = 4;
				/* do_nand and do_onenand do not check argv[0] */
				char *argv_erase[5]  = { NULL, "erase",  NULL, NULL, NULL, };

				argv_erase[2] = start;
				argv_erase[3] = length;

				sprintf(start, "0x%x", ptn->start);
				sprintf(length, "0x%x", ptn->length);

				if (ptn->length == 0)
					argc_erase = 3;

				status = CFG_FASTBOOT_FLASHCMD(NULL, 0, argc_erase, argv_erase);
#endif
			}

			if (status)
			{
				sprintf(response,"FAILfailed to erase partition");
			}
			else
			{
				printf("partition '%s' erased\n", ptn->name);
				sprintf(response, "OKAY");
			}
			ret = 0;
			goto send_tx_status;
		}

		/* download
		   download something ..
		   What happens to it depends on the next command after data */
		if (memcmp(cmdbuf, "download:", 9) == 0)
		{
			/* save the size */
			download_size = simple_strtoul (cmdbuf + 9, NULL, 16);
			/* Reset the bytes count, now it is safe */
			download_bytes = 0;
			/* Reset error */
			download_error = 0;

			printf ("Starting download of %d bytes\n", download_size);

			if (0 == download_size)
			{
				/* bad user input */
				sprintf(response, "FAILdata invalid size");
			}
			else if (download_size > interface.transfer_buffer_size)
			{
				/* set download_size to 0 because this is an error */
				download_size = 0;
				sprintf(response, "FAILdata too large");
			}
			else
			{
				/* The default case, the transfer fits
				   completely in the interface buffer */
				sprintf(response, "DATA%08x", download_size);
			}
			ret = 0;
			goto send_tx_status;
		}

		/* boot
		   boot what was downloaded

		   WARNING WARNING WARNING

		   This is not what you expect.
		   The fastboot client does its own packaging of the
		   kernel.  The layout is defined in the android header
		   file bootimage.h.  This layeout is copiedlooks like this,

		   **
		   ** +-----------------+
		   ** | boot header     | 1 page
		   ** +-----------------+
		   ** | kernel          | n pages
		   ** +-----------------+
		   ** | ramdisk         | m pages
		   ** +-----------------+
		   ** | second stage    | o pages
		   ** +-----------------+
		   **

		   What is a page size ?
		   The fastboot client uses 2048

		   The is the default value of CFG_FASTBOOT_MKBOOTIMAGE_PAGE_SIZE
		*/
		if (memcmp(cmdbuf, "boot", 4) == 0)
		{
			if ((download_bytes) &&
			    (CFG_FASTBOOT_MKBOOTIMAGE_PAGE_SIZE < download_bytes))
			{
				/* Note: We store zImage and ramdisk at different partitions */
				char addr_kernel[32];
				char addr_ramdisk[32];
				int pageoffset_ramdisk;

				char *bootm[3] = { "bootm", NULL, NULL, };
				//char *go[3]    = { "go",    NULL, NULL, };

				/*
				 * Use this later to determine if a command line was passed
				 * for the kernel.
				 */
				struct fastboot_boot_img_hdr *fb_hdr =
					(struct fastboot_boot_img_hdr *) interface.transfer_buffer;

				/* Skip the mkbootimage header */
				image_header_t *hdr =
					(image_header_t *)
					&interface.transfer_buffer[CFG_FASTBOOT_MKBOOTIMAGE_PAGE_SIZE];

				printf("Kernel size: %08x\n", fb_hdr->kernel_size);
				printf("Ramdisk size: %08x\n", fb_hdr->ramdisk_size);

				pageoffset_ramdisk = 1 + (fb_hdr->kernel_size + CFG_FASTBOOT_MKBOOTIMAGE_PAGE_SIZE - 1) / CFG_FASTBOOT_MKBOOTIMAGE_PAGE_SIZE;

				bootm[1] = addr_kernel;
				sprintf (addr_kernel, "0x%x", CFG_FASTBOOT_ADDR_KERNEL);
				memcpy((void *)CFG_FASTBOOT_ADDR_KERNEL,
					interface.transfer_buffer + CFG_FASTBOOT_MKBOOTIMAGE_PAGE_SIZE,
					fb_hdr->kernel_size);
				bootm[2] = addr_ramdisk;
				sprintf (addr_ramdisk, "0x%x", CFG_FASTBOOT_ADDR_RAMDISK);
				memcpy((void *)CFG_FASTBOOT_ADDR_RAMDISK, interface.transfer_buffer +
					(pageoffset_ramdisk * CFG_FASTBOOT_MKBOOTIMAGE_PAGE_SIZE),
					fb_hdr->ramdisk_size);

				/* Execution should jump to kernel so send the response
				   now and wait a bit.  */
				sprintf(response, "OKAY");
				fastboot_tx_status(response, strlen(response), FASTBOOT_TX_SYNC);
				udelay (1000000); /* 1 sec */

				if (ntohl(hdr->ih_magic) == IH_MAGIC) {
					/* Looks like a kernel.. */
					printf ("Booting kernel..\n");

					/*
					 * Check if the user sent a bootargs down.
					 * If not, do not override what is already there
					 */
					if (strlen ((char *) &fb_hdr->cmdline[0]))
						set_env ("bootargs", (char *) &fb_hdr->cmdline[0]);


        do_bootm (NULL, 0, 2, bootm);
				} else {
					/* Raw image, maybe another uboot */
					printf ("Booting raw image..\n");

					//do_go (NULL, 0, 2, go);
					do_bootm (NULL, 0, 3, bootm);
				}
				printf ("ERROR : bootting failed\n");
				printf ("You should reset the board\n");
			}
			sprintf(response, "FAILinvalid boot image");
			ret = 0;
		}

		/* flash
		   Flash what was downloaded */
		if (memcmp(cmdbuf, "flash:", 6) == 0)
		{
			if (download_bytes == 0)
			{
				sprintf(response, "FAILno image downloaded");
				ret = 0;
				goto send_tx_status;
			}

			struct fastboot_ptentry *ptn;

			LCD_setfgcolor(0x8B4500);
			LCD_setprogress(100);

			/* Special case: boot.img */
			if (!strcmp("boot", cmdbuf + 6))
			{
				int pageoffset_ramdisk;

				struct fastboot_boot_img_hdr *fb_hdr =
					(struct fastboot_boot_img_hdr *) interface.transfer_buffer;
				image_header_t *hdr =
					(image_header_t *)
					&interface.transfer_buffer[CFG_FASTBOOT_MKBOOTIMAGE_PAGE_SIZE];

				printf("Kernel size: %08x\n", fb_hdr->kernel_size);
				printf("Ramdisk size: %08x\n", fb_hdr->ramdisk_size);

				ptn = fastboot_flash_find_ptn("kernel");
				if (ptn->length && fb_hdr->kernel_size > ptn->length)
				{
					sprintf(response, "FAILimage too large for partition");
					goto send_tx_status;
				}

				if (OmPin == BOOT_ONENAND) {
#if defined(CFG_FASTBOOT_ONENANDBSP)
					ret = write_to_ptn(ptn,
						(unsigned int)interface.transfer_buffer + CFG_FASTBOOT_MKBOOTIMAGE_PAGE_SIZE,
						fb_hdr->kernel_size);
#endif
				} else if (OmPin == BOOT_MMCSD) {
					ret = write_to_ptn_sdmmc(ptn,
						(unsigned int)interface.transfer_buffer + CFG_FASTBOOT_MKBOOTIMAGE_PAGE_SIZE,
						fb_hdr->kernel_size);					
				} else if (OmPin == BOOT_EMMC441 || OmPin == BOOT_EMMC43) {
					ret = write_to_ptn_sdmmc(ptn,
						(unsigned int)interface.transfer_buffer + CFG_FASTBOOT_MKBOOTIMAGE_PAGE_SIZE,
						fb_hdr->kernel_size);					
				}

				pageoffset_ramdisk =
					1 + (fb_hdr->kernel_size + CFG_FASTBOOT_MKBOOTIMAGE_PAGE_SIZE - 1)
						/ CFG_FASTBOOT_MKBOOTIMAGE_PAGE_SIZE;
				ptn = fastboot_flash_find_ptn("ramdisk");
				if (ptn->length && fb_hdr->ramdisk_size > ptn->length)
				{
					sprintf(response, "FAILimage too large for partition");
					goto send_tx_status;
				}

				if (OmPin == BOOT_ONENAND) {
#if defined(CFG_FASTBOOT_ONENANDBSP)
					ret |= write_to_ptn(ptn,
						(unsigned int)interface.transfer_buffer + (pageoffset_ramdisk * CFG_FASTBOOT_MKBOOTIMAGE_PAGE_SIZE),
						fb_hdr->ramdisk_size);
#endif
				} else if (OmPin == BOOT_MMCSD) {
					ret |= write_to_ptn_sdmmc(ptn,
						(unsigned int)interface.transfer_buffer + (pageoffset_ramdisk * CFG_FASTBOOT_MKBOOTIMAGE_PAGE_SIZE),
						fb_hdr->ramdisk_size);
				} else if (OmPin == BOOT_EMMC441 || OmPin == BOOT_EMMC43) {
					ret |= write_to_ptn_sdmmc(ptn,
						(unsigned int)interface.transfer_buffer + (pageoffset_ramdisk * CFG_FASTBOOT_MKBOOTIMAGE_PAGE_SIZE),
						fb_hdr->ramdisk_size);
				}
				if (ret)
				{
					printf("flashing '%s' failed\n", "kernel+ramdisk");
					sprintf(response, "FAILfailed to flash partition");
				}
				else
				{
					printf("partition '%s' flashed\n", "kernel+ramdisk");
					sprintf(response, "OKAY");
				}

				ret = 0;
				goto send_tx_status;
			}

			ptn = fastboot_flash_find_ptn(cmdbuf + 6);
			if (ptn == 0)
			{
				sprintf(response, "FAILpartition does not exist");
			}
			else if ((download_bytes > ptn->length) && (ptn->length != 0) &&
				   !(ptn->flags & FASTBOOT_PTENTRY_FLAGS_WRITE_ENV))
			{
				sprintf(response, "FAILimage too large for partition");
				/* TODO : Improve check for yaffs write */
			}
			else
			{
				/* Check if this is not really a flash write
				   but rather a saveenv */
				if (ptn->flags & FASTBOOT_PTENTRY_FLAGS_WRITE_ENV)
				{
					/* Since the response can only be 64 bytes,
					   there is no point in having a large error message. */
					char err_string[32];
					if (saveenv_to_ptn(ptn, &err_string[0]))
					{
						printf("savenv '%s' failed : %s\n", ptn->name, err_string);
						sprintf(response, "FAIL%s", err_string);
					}
					else
					{
						printf("partition '%s' saveenv-ed\n", ptn->name);
						sprintf(response, "OKAY");
					}
				}
				else
				{
					/* Normal case */
					if (OmPin == BOOT_ONENAND) {
#if defined(CFG_FASTBOOT_ONENANDBSP)
						if (write_to_ptn(ptn, (unsigned int)interface.transfer_buffer, download_bytes))
						{
							printf("flashing '%s' failed\n", ptn->name);
							sprintf(response, "FAILfailed to flash partition");
						}
						else
						{
							printf("partition '%s' flashed\n", ptn->name);
							sprintf(response, "OKAY");
						}
#endif
					} else if (OmPin == BOOT_MMCSD) {
						if (write_to_ptn_sdmmc(ptn, (unsigned int)interface.transfer_buffer, download_bytes))
						{
							printf("flashing '%s' failed\n", ptn->name);
							sprintf(response, "FAILfailed to flash partition");
						}
						else
						{
							printf("partition '%s' flashed\n", ptn->name);
							sprintf(response, "OKAY");
						}

					} else if (OmPin == BOOT_EMMC441 || OmPin == BOOT_EMMC43) {
						if (write_to_ptn_sdmmc(ptn, (unsigned int)interface.transfer_buffer, download_bytes)) {
							printf("flashing '%s' failed\n", ptn->name);
							
							#if 1 //mj for emergency
							char run_cmd[40];
							sprintf(run_cmd,"emmc mode 0 0 1"); //change dev 0 to 1<<0 bit mode
							run_command(run_cmd, 0); //
							#endif
							
							sprintf(response, "FAILfailed to flash partition");
						} else {
							printf("partition '%s' flashed\n", ptn->name);
							sprintf(response, "OKAY");
						}

					}

				}
			}
			ret = 0;
			goto send_tx_status;
		}

		/* verify */
		/* continue */
		/* powerdown */

		/* oem
		   oem command. */
		if (memcmp(cmdbuf, "oem", 3) == 0)
		{
			sprintf(response,"INFOunknown OEM command");
			fastboot_tx_status(response, strlen(response), FASTBOOT_TX_ASYNC);

			sprintf(response,"OKAY");
			fastboot_tx_status(response, strlen(response), FASTBOOT_TX_ASYNC);

			return 0;
		}

send_tx_status:
		fastboot_tx_status(response, strlen(response), FASTBOOT_TX_ASYNC);

		LCD_setprogress(0);
	} /* End of command */

	return ret;
}


static int check_against_static_partition(struct fastboot_ptentry *ptn)
{
	int ret = 0;
	struct fastboot_ptentry *c;
	int i;

	for (i = 0; i < static_pcount; i++) {
		c = fastboot_flash_get_ptn((unsigned int) i);

		if (0 == ptn->length)
			break;

		if ((ptn->start >= c->start) &&
		    (ptn->start < c->start + c->length))
			break;

		if ((ptn->start + ptn->length > c->start) &&
		    (ptn->start + ptn->length <= c->start + c->length))
			break;

		if ((0 == strcmp(ptn->name, c->name)) &&
		    (0 == strcmp(c->name, ptn->name)))
			break;
	}

	if (i >= static_pcount)
		ret = 1;
	return ret;
}


static unsigned long long memparse(char *ptr, char **retptr)
{
	char *endptr;	/* local pointer to end of parsed string */

	unsigned long ret = simple_strtoul(ptr, &endptr, 0);

	switch (*endptr) {
	case 'M':
	case 'm':
		ret <<= 10;
	case 'K':
	case 'k':
		ret <<= 10;
		endptr++;
	default:
		break;
	}

	if (retptr)
		*retptr = endptr;

	return ret;
}


static int add_partition_from_environment(char *s, char **retptr)
{
	unsigned long size;
	unsigned long offset = 0;
	char *name;
	int name_len;
	int delim;
	unsigned int flags;
	struct fastboot_ptentry part;

	size = memparse(s, &s);
	if (0 == size) {
		printf("Error:FASTBOOT size of parition is 0\n");
		return 1;
	}

	/* fetch partition name and flags */
	flags = 0; /* this is going to be a regular partition */
	delim = 0;
	/* check for offset */
	if (*s == '@') {
		s++;
		offset = memparse(s, &s);
	} else {
		printf("Error:FASTBOOT offset of parition is not given\n");
		return 1;
	}

	/* now look for name */
	if (*s == '(')
		delim = ')';

	if (delim) {
		char *p;

		name = ++s;
		p = strchr((const char *)name, delim);
		if (!p) {
			printf("Error:FASTBOOT no closing %c found in partition name\n", delim);
			return 1;
		}
		name_len = p - name;
		s = p + 1;
	} else {
		printf("Error:FASTBOOT no partition name for \'%s\'\n", s);
		return 1;
	}

	/* test for options */
	while (1) {
		if (strncmp(s, "i", 1) == 0) {
			flags |= FASTBOOT_PTENTRY_FLAGS_WRITE_I;
			s += 1;
		} else if (strncmp(s, "yaffs", 5) == 0) {
			/* yaffs */
			flags |= FASTBOOT_PTENTRY_FLAGS_WRITE_YAFFS;
			s += 5;
		} else if (strncmp(s, "swecc", 5) == 0) {
			/* swecc */
			flags |= FASTBOOT_PTENTRY_FLAGS_WRITE_SW_ECC;
			s += 5;
		} else if (strncmp(s, "hwecc", 5) == 0) {
			/* hwecc */
			flags |= FASTBOOT_PTENTRY_FLAGS_WRITE_HW_ECC;
			s += 5;
		} else {
			break;
		}
		if (strncmp(s, "|", 1) == 0)
			s += 1;
	}

	/* enter this partition (offset will be calculated later if it is zero at this point) */
	part.length = size;
	part.start = offset;
	part.flags = flags;

	if (name) {
		if (name_len >= sizeof(part.name)) {
			printf("Error:FASTBOOT partition name is too long\n");
			return 1;
		}
		strncpy(&part.name[0], name, name_len);
		/* name is not null terminated */
		part.name[name_len] = '\0';
	} else {
		printf("Error:FASTBOOT no name\n");
		return 1;
	}


	/* Check if this overlaps a static partition */
	if (check_against_static_partition(&part)) {
		printf("Adding: %s, offset 0x%8.8x, size 0x%8.8x, flags 0x%8.8x\n",
		       part.name, part.start, part.length, part.flags);
		fastboot_flash_add_ptn(&part);
	}

	/* return (updated) pointer command line string */
	*retptr = s;

	/* return partition table */
	return 0;
}

#if defined(CONFIG_FASTBOOT)
static int set_partition_table()
{
	char fbparts[4096], *env;

	/*
	 * Place the runtime partitions at the end of the
	 * static paritions.  First save the start off so
	 * it can be saved from run to run.
	 */
	if (static_pcount >= 0)
	{
		/* Reset */
		pcount = static_pcount;
	}
	else
	{
		/* Save */
		static_pcount = pcount;
	}
	env = getenv("fbparts");
	if (env)
	{
		unsigned int len;
		len = strlen(env);
		if (len && len < 4096)
		{
			char *s, *e;

			memcpy(&fbparts[0], env, len + 1);
			printf("Fastboot: Adding partitions from environment\n");
			s = &fbparts[0];
			e = s + len;
			while (s < e)
			{
				if (add_partition_from_environment(s, &s))
				{
					printf("Error:Fastboot: Abort adding partitions\n");
					/* reset back to static */
					pcount = static_pcount;
					break;
				}
				/* Skip a bunch of delimiters */
				while (s < e)
				{
					if ((' ' == *s) ||
					    ('\t' == *s) ||
					    ('\n' == *s) ||
					    ('\r' == *s) ||
					    (',' == *s)) {
						s++;
					}
					else
					{
						break;
					}
				}
			}
		}
	}
	else if (ptable_default_size >= sizeof(fastboot_ptentry))
	{
		printf("Fastboot: employ default partition information\n");
		//memcpy(ptable, ptable_default, ptable_default_size);
		memcpy((void*)ptable, (void*)&ptable_default, ptable_default_size);
		pcount = ptable_default_size / sizeof(fastboot_ptentry);
	}
	else
	{
		printf("No partition informations!");
		return 0;
	}

#if 1 // Debug
	fastboot_flash_dump_ptn();
#endif

	LCD_setleftcolor(0x1024C0);

	return 0;
}
#endif

#if defined(CFG_FASTBOOT_SDMMCBSP)
static int set_partition_table_sdmmc()
{
	int start, count;
	unsigned char pid;

	pcount = 0;

#if defined(CONFIG_FUSED)
	/* FW BL1 for fused chip */
	strcpy(ptable[pcount].name, "fwbl1");
	ptable[pcount].start = 0;
	ptable[pcount].length = 0;
	ptable[pcount].flags = FASTBOOT_PTENTRY_FLAGS_USE_MOVI_CMD;
	pcount++;
#endif

	/* Bootloader */
	strcpy(ptable[pcount].name, "bootloader");
	ptable[pcount].start = 0;
	ptable[pcount].length = 0;
	ptable[pcount].flags = FASTBOOT_PTENTRY_FLAGS_USE_MOVI_CMD;
	pcount++;

	/* Kernel */
	strcpy(ptable[pcount].name, "kernel");
	ptable[pcount].start = 0;
	ptable[pcount].length = 0;
	ptable[pcount].flags = FASTBOOT_PTENTRY_FLAGS_USE_MOVI_CMD;
	pcount++;

	/* Ramdisk */
	strcpy(ptable[pcount].name, "ramdisk");
	ptable[pcount].start = 0;
	ptable[pcount].length = 0x300000;
	ptable[pcount].flags = FASTBOOT_PTENTRY_FLAGS_USE_MOVI_CMD;
	pcount++;

	/* Ramdisk */
	#ifdef CONFIG_RECOVERY //mj 
	strcpy(ptable[pcount].name, "Recovery");
	ptable[pcount].start = 0;
	ptable[pcount].length = 0x600000; //6MB
	ptable[pcount].flags = FASTBOOT_PTENTRY_FLAGS_USE_MOVI_CMD;
	pcount++;
	#endif

	/* System */
	get_mmc_part_info((dev_number_write == 0)?"0":"1", 2, &start, &count, &pid);
	if (pid != 0x83)
		goto part_type_error;
	strcpy(ptable[pcount].name, "system");
	ptable[pcount].start = start * CFG_FASTBOOT_SDMMC_BLOCKSIZE;
	ptable[pcount].length = count * CFG_FASTBOOT_SDMMC_BLOCKSIZE;
	ptable[pcount].flags = FASTBOOT_PTENTRY_FLAGS_USE_MMC_CMD;
	pcount++;

	/* Data */
	get_mmc_part_info((dev_number_write == 0)?"0":"1", 3, &start, &count, &pid);
	if (pid != 0x83)
		goto part_type_error;
	strcpy(ptable[pcount].name, "userdata");
	ptable[pcount].start = start * CFG_FASTBOOT_SDMMC_BLOCKSIZE;
	ptable[pcount].length = count * CFG_FASTBOOT_SDMMC_BLOCKSIZE;
	ptable[pcount].flags = FASTBOOT_PTENTRY_FLAGS_USE_MMC_CMD;
	pcount++;

	/* Cache */
	get_mmc_part_info((dev_number_write == 0)?"0":"1", 4, &start, &count, &pid);
	if (pid != 0x83)
		goto part_type_error;
	strcpy(ptable[pcount].name, "cache");
	ptable[pcount].start = start * CFG_FASTBOOT_SDMMC_BLOCKSIZE;
	ptable[pcount].length = count * CFG_FASTBOOT_SDMMC_BLOCKSIZE;
	ptable[pcount].flags = FASTBOOT_PTENTRY_FLAGS_USE_MMC_CMD;
	pcount++;

	/* fat */
	get_mmc_part_info((dev_number_write == 0)?"0":"1", 1, &start, &count, &pid);
	if (pid != 0xc)
		goto part_type_error;
	strcpy(ptable[pcount].name, "fat");
	ptable[pcount].start = start * CFG_FASTBOOT_SDMMC_BLOCKSIZE;
	ptable[pcount].length = count * CFG_FASTBOOT_SDMMC_BLOCKSIZE;
	ptable[pcount].flags = FASTBOOT_PTENTRY_FLAGS_USE_MMC_CMD;
	pcount++;

#if 1 // Debug
	fastboot_flash_dump_ptn();
#endif

	LCD_setleftcolor(0x8a2be2);

	return 0;

part_type_error:
	printf("Error: No MBR is found at SD/MMC.\n");
	printf("Hint: use fdisk command to make partitions.\n");

	return -1;
}
#endif

int do_fastboot (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int ret = 1;
	int check_timeout = 0;
	uint64_t timeout_endtime = 0;
	uint64_t timeout_ticks = 0;
	long timeout_seconds = -1;
	int continue_from_disconnect = 0;
	

/* checking boot mode before to set partition table	*/
/*
	switch(OmPin) {
		case BOOT_ONENAND:
			if (set_partition_table()) {
				return 1;
			}
			break;
		case BOOT_MMCSD:
		case BOOT_EMMC441:
		case BOOT_EMMC43:			
			if (set_partition_table_sdmmc()) {
				return 1;
			}
			break;
		case BOOT_NAND:
			if (set_partition_table()) {
				return 1;
			}
			break;
		}
	
#if  0
	if (set_partition_table())
		return 1;
#endif
*/


	if (1 == argc) {
		fuse_by_fastboot = 0;
		dev_number_write = 0;
	}
	else {
		fuse_by_fastboot = 1;
		dev_number_write = simple_strtoul(argv[1],NULL,10);
		if (!find_mmc_device(dev_number_write)) {
			fuse_by_fastboot = 0;
			dev_number_write = 0;
			return ret;
		}
	}
		

	/* checking boot mode before to set partition table	*/
	switch(OmPin) {
		case BOOT_ONENAND:
			if (set_partition_table()) {
				return 1;
			}
			break;
		case BOOT_MMCSD:
		case BOOT_EMMC441:
		case BOOT_EMMC43:			
			if (set_partition_table_sdmmc()) {
				fuse_by_fastboot = 0;
				dev_number_write = 0;
				return 1;
			}
			break;
		case BOOT_NAND:
			if (set_partition_table()) {
				return 1;
			}
			break;
		}
		
	/* Time out */
	if (3 == argc)	{
		long try_seconds;
		char *try_seconds_end;
		/* Check for timeout */
		try_seconds = simple_strtol(argv[2], &try_seconds_end, 10);
		if ((try_seconds_end != argv[2]) && (try_seconds >= 0))
		{
		check_timeout = 1;
		timeout_seconds = try_seconds;
		printf("Fastboot inactivity timeout %ld seconds\n", timeout_seconds);
		}
	}

	if (1 == check_timeout)
	{
		timeout_ticks = (uint64_t) (timeout_seconds * get_tbclk());
	}
		
		
	do
	{
		continue_from_disconnect = 0;

		/* Initialize the board specific support */
		if (0 == fastboot_init(&interface))
		{
			int poll_status;

			/* If we got this far, we are a success */
			ret = 0;

			timeout_endtime = get_ticks();
			timeout_endtime += timeout_ticks;

			LCD_turnon();

			while (1)
			{
				uint64_t current_time = 0;
				poll_status = fastboot_poll();

				if (1 == check_timeout)
					current_time = get_ticks();

				/* Check if the user wanted to terminate with ^C */
				if ((poll_status != FASTBOOT_OK) && (ctrlc()))
				{
					printf("Fastboot ended by user\n");
					continue_from_disconnect = 0;
					break;
				}

				if (FASTBOOT_ERROR == poll_status)
				{
					/* Error */
					printf("Fastboot error \n");
					break;
				}
				else if (FASTBOOT_DISCONNECT == poll_status)
				{
					/* break, cleanup and re-init */
					printf("Fastboot disconnect detected\n");
					continue_from_disconnect = 1;
					break;
				}
				else if ((1 == check_timeout) &&
					   (FASTBOOT_INACTIVE == poll_status))
				{
					/* No activity */
					if (current_time >= timeout_endtime)
					{
						printf("Fastboot inactivity detected\n");
						break;
					}
				}
				else
				{
					/* Something happened */
					/* Actual works of parsing are done by rx_handler */
					if (1 == check_timeout)
					{
						/* Update the timeout endtime */
						timeout_endtime = current_time;
						timeout_endtime += timeout_ticks;
					}
				}
			} /* while (1) */
		}

		/* Reset the board specific support */
		fastboot_shutdown();

		LCD_setfgcolor(0x000010);
		LCD_setleftcolor(0x000010);
		LCD_setprogress(100);

		/* restart the loop if a disconnect was detected */
	} while (continue_from_disconnect);
	dev_number_write = 0;
	fuse_by_fastboot = 0;
	return ret;
}

U_BOOT_CMD(
	fastboot,	3,	1,	do_fastboot,
	"fastboot- use USB Fastboot protocol\n",
	"<device num> [inactive timeout]\n"
	"    - Run as a fastboot usb device.\n"
	"    - The optional inactive timeout is the decimal seconds before\n"
	"    - the normal console resumes\n"
);


//#undef CONFIG_FASTBOOT_SDFUSE	// sdfuse is not implemented yet.
#ifdef CONFIG_FASTBOOT_SDFUSE

#include <part.h>
#include <fat.h>
//#define CFG_FASTBOOT_SDFUSE_DIR		"/"
#define CFG_FASTBOOT_SDFUSE_DIR		"/sdupdate/"
#define CFG_UPDATE_FILE_SRC_DEV		1 //MJ
#define CFG_BOOTING_DEV				0 //mj
//static int sdfuse_mmcdev=0;
#define CFG_FASTBOOT_SDFUSE_MMCPART	1
/*
 * part : partition name (This should be a defined name at ptable)
 * file : file to read
 */
static int update_from_sd (char *part, char *file)
{
	int ret = 1;

	/* Read file */
	if (file != NULL)
	{
		long size;
		unsigned long offset;
		unsigned long count;
		char filename[32];
		block_dev_desc_t *dev_desc=NULL;

		printf("Partition: %s, File: %s%s\n", part, CFG_FASTBOOT_SDFUSE_DIR, file);
		LCD_setfgcolor(0x2E8B57);
		LCD_setprogress(100);
		dev_desc = get_dev("mmc", CFG_UPDATE_FILE_SRC_DEV);
		if (dev_desc == NULL) {
			printf ("** Invalid boot device **\n");
			return 1;
		}
		if (fat_register_device(dev_desc, CFG_FASTBOOT_SDFUSE_MMCPART) != 0) {
			printf ("** Invalid partition **\n");
			return 1;
		}
		sprintf(filename, "%s%s", CFG_FASTBOOT_SDFUSE_DIR, file);
		offset = CFG_FASTBOOT_TRANSFER_BUFFER;
		count = 0;
		size = file_fat_read (filename, (unsigned char *) offset, count);

		if (size == -1) {
			printf("Failed to read %s\n", filename);
			return 1;
		}

		download_size = 0;	// should be 0
		download_bytes = size;

		printf ("%ld (0x%08x) bytes read\n", size, size);
	}
	else {
		printf("Partition: %s\n", part);

		download_size = 0;	// should be 0
		download_bytes = 0;
	}

	/* Write image into partition */
	/* If file is empty or NULL, just erase the part. */
	{
		char command[32];

		if (download_bytes == 0)
			sprintf(command, "%s:%s", "erase", part);
		else
			sprintf(command, "%s:%s", "flash", part);

		ret = rx_handler(command, sizeof(command));
	}

	return ret;
}

int sd_format_mmcs(void)
{
	int part = 1;
	block_dev_desc_t *dev_desc=NULL;

	dev_desc = get_dev("mmc", 0);
	if (dev_desc==NULL) {
		printf("\n** Invalid boot device **\n");
		return -1;
	}
	/*for t34, 1:fat, 2,3,4:ext4*/
	
	if(fat_format_device(dev_desc, part))
	{
		printf("fatformat failed for part_no:%d\n",part);
			return -1;
	}

	for(part = 2; part<=4; part++)
	{
		if(ext2fs_format(dev_desc, part))
		{
			printf("ext3 format failed for part_no:%d\n",part);
			return -1;
		}
	}
	return 0;
}

static int booting_dev_init(void)
{ //mj
	char command[32];
	block_dev_desc_t *dev_desc=NULL;

	emmcdbg("Initial the EMMC CARD booting area\n");

	sprintf(command, "fdisk -c %d",CFG_BOOTING_DEV);
	run_command(command,0);

	dev_desc = get_dev("mmc", 0);
	init_part(dev_desc);
	printf(" >>>part_type  : %x \n",dev_desc->part_type);
	if(sd_format_mmcs())
	{
		printf ("** sd_format_mmcs failed **\n");
		return -1;
	}
	return 0;
}
/* SD Fusing : read images from FAT partition of SD Card, and write it to boot device.
 *
 * NOTE
 * - sdfuse is not a original code of fastboot
 * - Fusing image from SD Card is not a original part of Fastboot protocol.
 * - This command implemented at this file to re-use an existing code of fastboot */
int do_sdfuse (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int ret = 1;
	int enable_reset = 0;
	struct mmc *mmc = NULL;
	
//ly
//	if (OmPin == BOOT_EMMC441 || OmPin == BOOT_EMMC43) 
		//sdfuse_mmcdev=1;
//	else
//		sdfuse_mmcdev=0;

	
	mmc = find_mmc_device(CFG_UPDATE_FILE_SRC_DEV);

	if (mmc == NULL) //mj
	{
		printf("*****************************\n");
		printf("NO Updating SD Card!\n");
		printf("*****************************\n");
		return -1;

	}
	else
	{
		ret = mmc_init(mmc);
		if (ret) {	
			printf("*****************************\n");
			printf("NO Updating SD Card!\n");
			printf("*****************************\n");
			return ret;
		}

	}

	interface.nand_block_size	= CFG_FASTBOOT_PAGESIZE * 64;
	interface.transfer_buffer	= (unsigned char *) CFG_FASTBOOT_TRANSFER_BUFFER;
	interface.transfer_buffer_size	= CFG_FASTBOOT_TRANSFER_BUFFER_SIZE;

	printf("[Fusing Image from SD Card.]\n");

#ifdef CFG_FASTBOOT_SDMMCBSP
	if ((argc == 2) && !strcmp(argv[1], "flashall")) //mj
	{
		if(booting_dev_init())
		{
			printf("*****************************\n");
			printf("Booting Device Initial Fail!\n");
			printf("*****************************\n");
			return -1;
		}
	}
	
	if (set_partition_table_sdmmc())
		return 1;
#else
	if (set_partition_table())
		return 1;
#endif

	if ((argc == 2) && !strcmp(argv[1], "info"))
	{
		printf("sdfuse will read images from the followings:\n");
		printf(" sd/mmc device  : mmc %d:%d\n",
			CFG_UPDATE_FILE_SRC_DEV, CFG_FASTBOOT_SDFUSE_MMCPART);
		printf(" directory      : %s\n", CFG_FASTBOOT_SDFUSE_DIR);
		printf(" booting device : %s\n",
#if defined(CFG_FASTBOOT_ONENANDBSP)
			"OneNAND"
#elif defined(CFG_FASTBOOT_NANDBSP)
			"NAND"
#elif defined(CFG_FASTBOOT_SDMMCBSP)
			"SD/MMC/eMMC"
#else
#error "Unknown booting device!"
#endif
#if defined(CONFIG_FUSED)
			" (on eFused Chip)"
#endif
		);
		return 0;
	}
	else if ((argc == 2) && !strcmp(argv[1], "flashall"))
	{
		LCD_turnon();

		//if (update_from_sd("boot", "boot.img"))
		if (s5pc210_cpu_id  == SMDK4212_AP10_ID) {
			if (update_from_sd("bootloader", CONFIG_4212_AP10_BOOTLOADER))
				printf("*****[bootloader] image updating failed...");
		}
		else if (s5pc210_cpu_id  == SMDK4212_AP11_ID) {
			if (update_from_sd("bootloader", CONFIG_4212_AP11_BOOTLOADER))
				printf("*****[bootloader] image updating failed...");
		}
		else if ((s5pc210_cpu_id & 0xfffff000) == SMDK4412_ID && Is_TC4_Dvt == 0) {
			if (update_from_sd("bootloader", CONFIG_4412_BOOTLOADER))
				printf("*****[bootloader] image updating failed...");
		}
		else if ((s5pc210_cpu_id & 0xfffff000) == SMDK4412_ID && Is_TC4_Dvt != 0) {
			if (update_from_sd("bootloader", CONFIG_4412_DVT_BOOTLOADER))
				printf("*****[bootloader] image updating failed...");
		}
		else 
		{
			printf("*****[ERROR] This is unknown board...");
			goto err_sdfuse;
		}
		
		if (update_from_sd("kernel", "zImage"))
			printf("*****[kernel] image updating failed...");

		if (update_from_sd("ramdisk", "ramdisk-uboot.img"))
			printf("*****[ramdisk] image updating failed...");

		#ifdef CONFIG_RECOVERY
		if (update_from_sd("Recovery", "ramdisk-recovery-uboot.img"))
			printf("*****[Recovery] image updating failed...");
		#endif
		
		if (update_from_sd("system", "system.img"))
			printf("*****[system] image updating failed...");
		
		if (update_from_sd("userdata", NULL))
			goto err_sdfuse;
		if (update_from_sd("cache", NULL))
			goto err_sdfuse;
		if (update_from_sd("fat", NULL))
			goto err_sdfuse;

		//enable_reset = 1; //mj current reset has problem
		ret = 0;
	}
	else if ((argc == 4) && !strcmp(argv[1], "flash"))
	{
		LCD_turnon();

		if (update_from_sd(argv[2], argv[3]))
			goto err_sdfuse;

		ret = 0;
	}
	else if ((argc == 3) && !strcmp(argv[1], "erase"))
	{
		LCD_turnon();

		if (update_from_sd(argv[2], NULL))
			goto err_sdfuse;

		ret = 0;
	}
	else
	{
		printf("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}



err_sdfuse:
	LCD_setfgcolor(0x000010);
	LCD_setleftcolor(0x000010);
	LCD_setprogress(100);

	if (enable_reset)
		do_reset (NULL, 0, 0, NULL);

	return ret;
}

U_BOOT_CMD(
	sdfuse,		4,	1,	do_sdfuse,
	"sdfuse  - read images from FAT partition of SD card and write them to booting device.\n",
	"info                             - print primitive infomation.\n"
	"sdfuse flashall                         - flash boot.img, system.img,\n"
	"                                          erase userdata, cache, and reboot.\n"
	"sdfuse flash <partition> [ <filename> ] - write a file to a partition.\n"
	"sdfuse erase <partition>                - erase (format) a partition.\n"
);
#endif	// CONFIG_FASTBOOT_SDFUSE


/*
 * Android style flash utilties */
void fastboot_flash_add_ptn(fastboot_ptentry *ptn)
{
	if (pcount < MAX_PTN)
	{
		memcpy(ptable + pcount, ptn, sizeof(*ptn));
		pcount++;
	}
}

void fastboot_flash_dump_ptn(void)
{
	unsigned int n;

	printf("[Partition table on ");

	switch(OmPin) {
		case BOOT_ONENAND:
			printf("OneNAND");
			break;
		case BOOT_MMCSD:
		case BOOT_EMMC441:
		case BOOT_EMMC43:
			printf("MoviNAND");
			break;
		case BOOT_NAND:
			printf("NAND");
			break;
		}
	printf("]\n");

	for (n = 0; n < pcount; n++)
	{
		fastboot_ptentry *ptn = ptable + n;
#if 0	/* old format - decimal */
		printf("ptn %d name='%s' start=%d len=%d\n",
				n, ptn->name, ptn->start, ptn->length);
#else
		printf("ptn %d name='%s' ", n, ptn->name);
		if (n == 0 || ptn->start)
			printf("start=0x%X ", ptn->start);
		else
			printf("start=N/A ");
		if (ptn->length)
			printf("len=0x%X(~%dKB) ", ptn->length, ptn->length>>10);
		else
			printf("len=N/A ");

		
		if (ptn->flags & FASTBOOT_PTENTRY_FLAGS_WRITE_YAFFS)
			printf("(Yaffs)\n");
		else if (ptn->flags & FASTBOOT_PTENTRY_FLAGS_USE_MOVI_CMD)
			printf("(use hard-coded info. (cmd: movi))\n");
		else
			printf("\n");
#endif
	}
}


fastboot_ptentry *fastboot_flash_find_ptn(const char *name)
{
	unsigned int n;

	for (n = 0; n < pcount; n++)
	{
		/* Make sure a substring is not accepted */
		if (strlen(name) == strlen(ptable[n].name))
		{
			if (0 == strcmp(ptable[n].name, name))
				return ptable + n;
		}
	}
	return 0;
}

fastboot_ptentry *fastboot_flash_get_ptn(unsigned int n)
{
	if (n < pcount) {
		return ptable + n;
	} else {
		return 0;
	}
}

unsigned int fastboot_flash_get_ptn_count(void)
{
	return pcount;
}



#endif	/* CONFIG_FASTBOOT */


