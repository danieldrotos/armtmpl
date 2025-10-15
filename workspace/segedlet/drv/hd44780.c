#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "dev_gpio.h"
#include "ols_driver.h"
#include "ols.h"
#include "monitor.h"
#include "temu.h"

#include "hd44780.h"

static uint32_t *rs, *rw, *e;
static uint32_t *d0o, *d1o, *d2o, *d3o, *d4o, *d5o, *d6o, *d7o;
static uint32_t *d0i, *d1i, *d2i, *d3i, *d4i, *d5i, *d6i, *d7i;

static int diso= 0, opened= 0;

static struct temu_t hd44780_te;


static void
hd44780_pulse()
{
  volatile int i;
  *e= 1;
  for (i= 0; i<2; i++) ;
  *e= 0;
}

/*
static void
hd44780_pulse2(int l)
{
  volatile int i;
  *e= 1;
  for (i= 0; i<l; i++) ;
  *e= 0;
}
*/

static void
hd44780_dir_out()
{
  dev_gpio_output(D0);
  dev_gpio_output(D1);
  dev_gpio_output(D2);
  dev_gpio_output(D3);
  dev_gpio_output(D4);
  dev_gpio_output(D5);
  dev_gpio_output(D6);
  dev_gpio_output(D7);
  diso= 1;
}

static void
hd44780_dir_in()
{
  dev_gpio_input(D0);
  dev_gpio_input(D1);
  dev_gpio_input(D2);
  dev_gpio_input(D3);
  dev_gpio_input(D4);
  dev_gpio_input(D5);
  dev_gpio_input(D6);
  dev_gpio_input(D7);
  diso= 0;
}

static uint8_t
hd44780_get_in()
{
  uint8_t d= 0;
  if (*d7i) d|= 0x80;
  if (*d6i) d|= 0x40;
  if (*d5i) d|= 0x20;
  if (*d4i) d|= 0x10;
  if (*d3i) d|= 0x08;
  if (*d2i) d|= 0x04;
  if (*d1i) d|= 0x02;
  if (*d0i) d|= 0x01;
  return d;
}

static uint8_t
hd44780_get_out()
{
  uint8_t d= 0;
  if (*d7o) d|= 0x80;
  if (*d6o) d|= 0x40;
  if (*d5o) d|= 0x20;
  if (*d4o) d|= 0x10;
  if (*d3o) d|= 0x08;
  if (*d2o) d|= 0x04;
  if (*d1o) d|= 0x02;
  if (*d0o) d|= 0x01;
  return d;
}

static int hd44780_wait(int tout);

static uint8_t
hd44780_rd(int data)
{
  uint8_t d;
  if (data)
    {
      if (hd44780_wait(2))
        return ' ';
    }
  if (diso) hd44780_dir_in();
  *rw= 1;
  *rs= data?1:0;
  *e= 1;
  d= hd44780_get_in();
  *e= 0;
  *rw= 0;
  return d;
}

// TRUE if busy
static int
hd44780_wait(int tout)
{
  unsigned int start= HAL_GetTick();
  uint8_t d= hd44780_rd(0);
  while ((d & 0x80) && (HAL_GetTick() - start < tout))
    d= hd44780_rd(0);
  return d & 0x80;
}

static void
hd44780_set_out(uint8_t d)
{
  *d7o= (d & 0x80)?1:0;
  *d6o= (d & 0x40)?1:0;
  *d5o= (d & 0x20)?1:0;
  *d4o= (d & 0x10)?1:0;
  *d3o= (d & 0x08)?1:0;
  *d2o= (d & 0x04)?1:0;
  *d1o= (d & 0x02)?1:0;
  *d0o= (d & 0x01)?1:0;
}

// TRUE if timeouted
static int
hd44780_wr(int data, uint8_t d)
{
  if (hd44780_wait(2)) return 1;
  *rs= data?1:0;
  *rw= 0;
  if (!diso) hd44780_dir_out();
  hd44780_set_out(d);
  hd44780_pulse();
  hd44780_dir_in();
  return 0;
}

void
hd44780_init()
{
  dev_gpio_soutput(E, 0);
  dev_gpio_soutput(RS, 0);
  dev_gpio_soutput(RW, 0);

  rs= dev_gpio_band_output(RS);
  rw= dev_gpio_band_output(RW);
  e = dev_gpio_band_output(E);

  hd44780_dir_in();

  d0o= dev_gpio_band_output(D0);
  d0i= dev_gpio_band_input (D0);
  d1o= dev_gpio_band_output(D1);
  d1i= dev_gpio_band_input (D1);
  d2o= dev_gpio_band_output(D2);
  d2i= dev_gpio_band_input (D2);
  d3o= dev_gpio_band_output(D3);
  d3i= dev_gpio_band_input (D3);
  d4o= dev_gpio_band_output(D4);
  d4i= dev_gpio_band_input (D4);
  d5o= dev_gpio_band_output(D5);
  d5i= dev_gpio_band_input (D5);
  d6o= dev_gpio_band_output(D6);
  d6i= dev_gpio_band_input (D6);
  d7o= dev_gpio_band_output(D7);
  d7i= dev_gpio_band_input (D7);

  // Function set: 001 D N F - -
  //               001 1 1 1 0 0
  hd44780_wr(0, 0x3c);
  // Display on/off 00001 D C B
  //                00001 1 0 0
  hd44780_wr(0, 0x0c);
  // Entry mode 000001 ID S
  //            000001 1  0
  hd44780_wr(0, 0x06);
  // clear
  hd44780_wr(0, 1);
  // home
  hd44780_wr(0, 2);
  hd44780_wr(0, 0x80);

  hd44780_te.cols= 16;
  hd44780_te.rows= 2;
  hd44780_te.curs_x= 0;
  hd44780_te.curs_y= 0;
  hd44780_te.esc_state= 0;
  hd44780_te.nuof_fonts= 2;
  hd44780_te.sgr_font= 1;
}

static uint8_t
hd44780_addr_of(int x, int y)
{
  int a= ((y==1)?0x40:0) + (x&0x3f);
  return a;
}

static void
hd44780_set_addr(int x, int y)
{
  uint8_t a= hd44780_addr_of(x, y);
  hd44780_wr(0, 0x80 | a);
}

static void
hd44780_clear_line(int l)
{
  int i;
  hd44780_set_addr(0, l);
  for (i= 0; i<hd44780_te.cols; i++)
    hd44780_wr(1, ' ');
}

static void
hd44780_scroll()
{
  if (hd44780_te.curs_x >= hd44780_te.cols)
    {
      hd44780_te.curs_x= 0;
      hd44780_te.curs_y+= 1;
    }
  while (hd44780_te.curs_y >= hd44780_te.rows)
    {
      // scroll up
      int l;
      for (l= 1; l < hd44780_te.rows; l++)
        {
          char lin[256];
          int x;
          hd44780_set_addr(0, l);
          for (x=0;x<hd44780_te.cols;x++)
            lin[x]= hd44780_rd(1);
          hd44780_set_addr(0, l-1);
          for (x=0;x<hd44780_te.cols;x++)
            hd44780_wr(1, lin[x]);
        }
      hd44780_clear_line(hd44780_te.rows-1);
      hd44780_te.curs_y--;
    }
}

static void
hd44780_display(char c)
{
  hd44780_set_addr(hd44780_te.curs_x, hd44780_te.curs_y);
  hd44780_wr(1, c);
  hd44780_te.curs_x++;
  hd44780_scroll();
}

static void
hd44780_printc(char c)
{
  enum temu_ret_t i= temu_print(&hd44780_te, c);
  int l, x;
  switch (i)
  {
    case temu_clear_below_cursor:
      for (l= hd44780_te.curs_y+1; l < hd44780_te.rows; l++)
        hd44780_clear_line(l);
      break;
    case temu_clear_above_cursor:
      for (l= 0; l < hd44780_te.curs_y; l++)
        hd44780_clear_line(l);
      break;
    case temu_clear_entire_screen:
      hd44780_wr(0, 1);
      break;
    case temu_clear_left:
      hd44780_set_addr(hd44780_te.curs_x, hd44780_te.curs_y);
      for (x= hd44780_te.curs_x; x < hd44780_te.cols; x++)
        {
          hd44780_wr(1, ' ');
        }
      break;
    case temu_clear_right:
      hd44780_set_addr(0, hd44780_te.curs_y);
      for (x= 0; x <= hd44780_te.curs_x; x++)
        {
          hd44780_wr(1, ' ');
        }
      break;
    case temu_clear_line:
      hd44780_clear_line(hd44780_te.curs_y);
      break;
    case temu_set_sgr:
      hd44780_te.sgr_font&= 1;
      // Function set: 001 D N F - -
      //               001 1 1 F 0 0
      hd44780_wr(0, 0x38|(hd44780_te.sgr_font?4:0));
      break;
    case temu_show_cursor:
      // Display on/off 00001 D C B
      //                00001 1 1 1
      hd44780_wr(0, 0x0f);
      break;
    case temu_hide_cursor:
      // Display on/off 00001 D C B
      //                00001 1 0 0
      hd44780_wr(0, 0x0c);
      break;
  }
  hd44780_scroll();
  if ((i > 0) && isprint(c))
    {
      hd44780_display(c);
    }
  hd44780_set_addr(hd44780_te.curs_x, hd44780_te.curs_y);
}

void
hd44780_printf(char *fmt, ...)
{
  char buf[200], *p;
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(buf, 199, fmt, ap);
  for (p= buf; *p; p++)
    hd44780_printc(*p);
}

void
hd44780_info(int fd)
{
  int i;
  dprintf(fd, "D = %s (out=%02x, in=%02x)\n", diso?"out":"in",
      hd44780_get_out(), hd44780_get_in());
  i= *e;
  dprintf(fd, "E = %d\n", i);
  i= *rs;
  dprintf(fd, "RS= %d %s\n", i, i?"data":"cmd");
  i= *rw;
  dprintf(fd, "RW= %d %s\n", i, i?"RD":"WR");
}

int
hd44780_cmd(int fd, char *cmd)
{
  char *s;
  int i;
  if (strcmp(cmd, "E")==0)
    {
      if ((s= strtok(NULL, DELIMS))!=NULL)
        {
          i= strtol(s, 0, 0);
          *e= i;
        }
      dprintf(fd, "E=%d\n", (int)(*e));
    }
  else if (strcmp(cmd, "W")==0)
    {
      i= 0;
      if ((s= strtok(NULL, DELIMS))!=NULL)
        {
          i= strtol(s, 0, 0);
        }
      *rw= i;
      i= *rw;
      dprintf(fd, "RW=%d (%s)\n", i, i?"RD":"WR");
    }
  else if (strcmp(cmd, "R")==0)
    {
      i= 1;
      if ((s= strtok(NULL, DELIMS))!=NULL)
        {
          i= strtol(s, 0, 0);
        }
      *rw= i;
      i= *rw;
      dprintf(fd, "RW=%d (%s)\n", i, i?"RD":"WR");
    }
  else if ((strcmp(cmd, "data")==0) ||
      (strcmp(cmd, "cmd")==0) ||
      (strcmp(cmd, "rs")==0))
    {
      if (*cmd=='d')
        *rs= 1;
      else if (*cmd=='c')
        *rs= 0;
      i= *rs;
      dprintf(fd, "RS=%d %s\n", i, i?"data":"cmd");
    }
  else if (strcmp(cmd, "IN")==0)
    {
      hd44780_dir_in();
      hd44780_info(fd);
    }
  else if (strcmp(cmd, "OUT")==0)
    {
      i= 0;
      if ((s= strtok(NULL, DELIMS))!=NULL)
        i= strtol(s, 0, 0);
      hd44780_set_out(i);
      hd44780_dir_out();
      hd44780_info(fd);
    }
  else if (strcmp(cmd, "set")==0)
    {
      if ((s= strtok(NULL, DELIMS))!=NULL)
        {
          i= strtol(s, 0, 0);
          hd44780_set_out(i);
        }
      hd44780_info(fd);
    }
  else if (strcmp(cmd, "wr")==0)
    {
      if ((s= strtok(NULL, DELIMS))!=NULL)
        {
          i= strtol(s, 0, 0);
          i= hd44780_wr(0, i);
          dprintf(fd, "%s\n", i?"fail":"OK");
          hd44780_info(fd);
        }
    }
  else if (strcmp(cmd, "pr")==0)
    {
      if ((s= strtok(NULL, DELIMS))!=NULL)
        {
          i= *s;//strtol(s, 0, 0);
          i= hd44780_wr(1, i);
          dprintf(fd, "%s\n", i?"fail":"OK");
          hd44780_info(fd);
        }
    }
  else if (strcmp(cmd, "rd")==0)
    {
      i= hd44780_rd(0);
      dprintf(fd, "d=0x%02x\n", i);
      dprintf(fd, "BF=%d\n", (i&0x80)?1:0);
      i&= 0x7f;
      dprintf(fd, "AD=%d 0x%02x\n", i, i);
      hd44780_info(fd);
    }
  else if (strcmp(cmd, "stat")==0)
    {
      hd44780_info(fd);
    }
  else return 0;
  return 1;
}


/* File system interface */

static int
dev_hd44780_open(int device, int fid, int fo_flags)
{
  hd44780_init();
  opened= 1;
  return 0;
}

static int
dev_hd44780_close(int device, int fid)
{
  opened= 0;
  return 0;
}

static int
dev_hd44780_write(int device, int fid, void *buf, int nr)
{
  int i;
  if (!opened)
    return 0;
  for (i= 0; i<nr; i++)
    hd44780_printc(((uint8_t*)buf)[i]);
  return nr;
}

static int
dev_hd44780_ioctl(int device, int fid, unsigned long int request, va_list ap)
{
  return -1;
}

static const struct ols_driver_entry hd44780= {
  .name= "/dev/hd44780",
  .is_tty= 1,
  .open_fn= dev_hd44780_open,
  .read_fn= NULL,
  .write_fn= dev_hd44780_write,
  .ioctl_fn= dev_hd44780_ioctl,
  .close_fn= dev_hd44780_close,
  .seek_fn= NULL,
  .input_avail_fn= NULL,
  .writable_fn= NULL
};

int
ols_drv_hd44780_install(void)
{
  ols_register_driver(&hd44780);
  return 0;
}
