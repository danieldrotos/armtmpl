#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>

#include "wrap_termios.h"
#include "wrap_ioctl.h"

#include "ols_driver.h"
#include "ols.h"

#include "dev_usart.h"

#include "ols_drv_usart.h"
/*
static const struct ols_driver_entry usart= {
  .name= "/dev/usart",
  .is_tty= 1,
  .open_fn= f4_usart_open,
  .read_fn= f4_usart_read,
  .write_fn= f4_usart_write,
  .ioctl_fn= f4_usart_ioctl,
  .close_fn= f4_usart_close,
  .seek_fn= NULL,
  .input_avail_fn= f4_usart_input_avail,
  .writable_fn= f4_usart_tx_free_space
};
*/
static const struct ols_driver_entry /*ttest*/usart= {
  .name= "/dev/usart",///dev/ttest",
  .is_tty= 1,
  .open_fn= dev_ttest_open,
  .read_fn= dev_ttest_read,
  .write_fn= dev_ttest_write,
  .ioctl_fn= dev_usart_ioctl,
  .close_fn= dev_ttest_close,
  .seek_fn= NULL,
  .input_avail_fn= dev_ttest_input_avail,
  .writable_fn= NULL
};


int
ols_drv_usart_install(void)
{
	/*
	int i;
  for (i= 0; i<USART_NUOF_DEVICES; i++)
    {
      memset(&(usart_devices[i].termio), 0, sizeof(struct termios));
      cfsetspeed(&(usart_devices[i].termio), 38400);
      usart_devices[i].termio.c_cflag|= CLOCAL | CREAD | CS8;
      usart_devices[i].termio.c_cc[VEOF]	= 4;
      usart_devices[i].termio.c_cc[VERASE]	= 127;
      usart_devices[i].termio.c_cc[VWERASE]	= 23;
      usart_devices[i].termio.c_cc[VKILL]	= 21;
      usart_devices[i].termio.c_cc[VREPRINT]	= 18;
      usart_devices[i].termio.c_cc[VINTR]	= 3;
      usart_devices[i].termio.c_cc[VQUIT]	= 28;
      usart_devices[i].termio.c_cc[VSUSP]	= 26;
      usart_devices[i].termio.c_cc[VDSUSP]	= 25;
      usart_devices[i].termio.c_cc[VSTART]	= 17;
      usart_devices[i].termio.c_cc[VSTOP]	= 19;
      usart_devices[i].termio.c_cc[VLNEXT]	= 22;
    }
    */
  //usart_devices[1].termio.c_lflag|= ECHO;
  ols_register_driver(&usart);
  //ols_register_driver(&ttest);
  return 0;
}
