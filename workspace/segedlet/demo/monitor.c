#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "dev_gpio.h"
#include "ols_fs.h"
#include "terminal.h"

#include "monitor.h"

static char cmd[CMD_LEN]= { 0 };

struct tu_input_t mon= {
		.raw_mode= 0,
		.line= { 0 }
};

__attribute__((weak)) int app_cmd(int fd, char *cmd) {return 0;}

static int
proc_command(int fd)
{
	char *s= strtok(cmd, DELIMS);
	dprintf(fd, "\n");
	if (s && *s)
    {
		if (strcmp(s, "test") == 0)
		{
			dprintf(fd, "OK\n");
		}
		else if (strcmp(s, "gpio") == 0)
		{
			char *p= strtok(NULL, DELIMS);
			if (p && *p)
			{
				dev_print_gpio(fd, p);
			}
		}
		else if (strcmp(s, "cls")==0)
				dtu_clear_screen(fd);
		else if (strcmp(s, "QUIT") == 0)
			return 1;
		else if (app_cmd(fd, s))
			return 0;
		else
			dprintf(fd, "What?\n");
    }
  return 0;
}

int
monitor(int ifd, int ofd)
{
	int r= 0;
	mon.ifd= ifd;
	mon.ofd= ofd;
	if (ti_gets(&mon, cmd, CMD_LEN))
    {
		r= proc_command(ofd);
    }
  return r;
}

/* End of monitor.c */
