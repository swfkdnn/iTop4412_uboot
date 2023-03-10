#ifndef __COMMAND_H
#define __COMMAND_H

//#include <config.h>
#include "/home/sw/pp_self/1/iTop4412_uboot/sw_build/include/config.h"

#ifndef NULL
#define NULL	0
#endif

/* Default to a width of 8 characters for help message command width */
#ifndef CONFIG_SYS_HELP_CMD_WIDTH
#define CONFIG_SYS_HELP_CMD_WIDTH	8
#endif

#ifndef	__ASSEMBLY__
/*
 * Monitor Command Table
 */

struct cmd_tbl_s {
	char		*name;                      /* Command Name			*/
	int		maxargs;                      /* maximum number of arguments	*/
	int		repeatable;                   /* autorepeat allowed?		*/
/* Implementation function	*/
	int		(*cmd)(struct cmd_tbl_s *, int, int, char *[]);
	char		*usage;                     /* Usage message	(short)	*/

#define  CONFIG_SYS_LONGHELP
#ifdef	CONFIG_SYS_LONGHELP
	char		*help;                      /* Help  message	(long)	*/
#endif

#ifdef CONFIG_AUTO_COMPLETE
/* do auto completion on the arguments */
	int		(*complete)(int argc, char *argv[], char last_char, int maxv, char *cmdv[]);
#endif

};

typedef struct cmd_tbl_s	cmd_tbl_t;

extern cmd_tbl_t  __u_boot_cmd_start;
extern cmd_tbl_t  __u_boot_cmd_end;


/* common/command.c */
int _do_help (cmd_tbl_t *cmd_start, int cmd_items, cmd_tbl_t * cmdtp, int
	      flag, int argc, char *argv[]);
cmd_tbl_t *find_cmd(const char *cmd);
cmd_tbl_t *find_cmd_tbl (const char *cmd, cmd_tbl_t *table, int table_len);

extern int cmd_usage(cmd_tbl_t *cmdtp);

#ifdef CONFIG_AUTO_COMPLETE
extern void install_auto_complete(void);
extern int cmd_auto_complete(const char *const prompt, char *buf, int *np, int *colp);
#endif

/*
 * Monitor Command
 *
 * All commands use a common argument format:
 *
 * void function (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
 */

typedef	void	command_t (cmd_tbl_t *, int, int, char *[]);

#if defined(CONFIG_CMD_MEMORY)		\
    || defined(CONFIG_CMD_I2C)		\
    || defined(CONFIG_CMD_ITEST)	\
    || defined(CONFIG_CMD_PCI)		\
    || defined(CONFIG_CMD_PORTIO)
#define CMD_DATA_SIZE
extern int cmd_get_data_size(char* arg, int default_size);
#endif

#endif	/* __ASSEMBLY__ */

/*
 * Command Flags:
 */
#define CMD_FLAG_REPEAT		0x0001	/* repeat last command		*/
#define CMD_FLAG_BOOTD		0x0002	/* command is from bootd	*/

#define Struct_Section  __attribute__ ((unused,section (".u_boot_cmd")))

#ifdef  CONFIG_SYS_LONGHELP
#define U_BOOT_CMD(name,maxargs,rep,cmd,usage,help) \
cmd_tbl_t __u_boot_cmd_##name Struct_Section = {#name, maxargs, rep, cmd, usage, help}
#define U_BOOT_CMD_MKENT(name,maxargs,rep,cmd,usage,help) \
{#name, maxargs, rep, cmd, usage, help}
#else	/* no long help info */
#define U_BOOT_CMD(name,maxargs,rep,cmd,usage,help) \
cmd_tbl_t __u_boot_cmd_##name Struct_Section = {#name, maxargs, rep, cmd, usage}
#define U_BOOT_CMD_MKENT(name,maxargs,rep,cmd,usage,help) \
{#name, maxargs, rep, cmd, usage}
#endif	/* CONFIG_SYS_LONGHELP */

#endif	/* __COMMAND_H */
