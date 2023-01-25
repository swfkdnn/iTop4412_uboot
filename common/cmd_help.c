#include <common.h>
#include <command.h>

int do_help(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
  printf("[%s %s] %s: %s: %d\n", \
  __DATE__, __TIME__, __FILE__, __func__, __LINE__);
	return _do_help(&__u_boot_cmd_start,
			((cmd_tbl_t *)(&__u_boot_cmd_end) - (cmd_tbl_t *)(&__u_boot_cmd_start))/sizeof(cmd_tbl_t *),
			cmdtp, flag, argc, argv);
}

U_BOOT_CMD(
	help,	CONFIG_SYS_MAXARGS,	1,	do_help,
	"print command description/usage",
	"\n"
	"	- print brief description of all commands\n"
	"help command ...\n"
	"	- print detailed usage of 'command'"
);

/* This does not use the U_BOOT_CMD macro as ? can't be used in symbol names */
cmd_tbl_t __u_boot_cmd_question_mark Struct_Section = {
	"?",	CONFIG_SYS_MAXARGS,	1,	do_help,
	"alias for 'help'",
#ifdef  CONFIG_SYS_LONGHELP
	""
#endif /* CONFIG_SYS_LONGHELP */
};
