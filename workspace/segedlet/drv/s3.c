#include <stdio.h>

#include "brd.h"
#include "dev_gpio.h"

#include "s3.h"


static void
f4_delay(int ms)
{
  unsigned int n= HAL_GetTick();

  while ((HAL_GetTick() - n) < ms)
    ;
}

/* Init GPIO for bus lines */
void
s3_init(void)
{
  /* Connected, but unused pins */
  dev_gpio_input("PD13");
  dev_gpio_input("PD11");
  dev_gpio_input("PE15");
  dev_gpio_input("PB10");
  dev_gpio_input("PB1");
  dev_gpio_input("PB0");
  dev_gpio_input("PC5");
  dev_gpio_input("PC4");
  dev_gpio_input("PA7");
  dev_gpio_input("PA6");
  dev_gpio_input("PA5");
  dev_gpio_input("PA4");

  /* Control lines */
  dev_gpio_output(S_RST);
  dev_gpio_set(S_RST, 0);
  dev_gpio_output(S_WR);
  dev_gpio_set(S_WR, 0);
  dev_gpio_output(S_RD);
  dev_gpio_set(S_RD, 0);

  /* Data lines */
  dev_gpio_input(S_D0);
  dev_gpio_input(S_D1);
  dev_gpio_input(S_D2);
  dev_gpio_input(S_D3);
  dev_gpio_input(S_D4);
  dev_gpio_input(S_D5);
  dev_gpio_input(S_D6);
  dev_gpio_input(S_D7);

  /* Address lines */
  dev_gpio_output(S_A0);
  dev_gpio_output(S_A1);
  dev_gpio_output(S_A2);
  dev_gpio_output(S_A3);
  dev_gpio_output(S_A4);
  dev_gpio_output(S_A5);
  dev_gpio_output(S_A6);
  dev_gpio_output(S_A7);

  /* Send a RESET */
  dev_gpio_set(S_RST, 1);
  f4_delay(1);
  dev_gpio_set(S_RST, 0);
}

/* Turn ON data bus */
static void
s3_d_on()
{
  dev_gpio_output(S_D0);
  dev_gpio_output(S_D1);
  dev_gpio_output(S_D2);
  dev_gpio_output(S_D3);
  dev_gpio_output(S_D4);
  dev_gpio_output(S_D5);
  dev_gpio_output(S_D6);
  dev_gpio_output(S_D7);
}

/* Turn OFF data bus */
static void
s3_d_off()
{
  dev_gpio_input(S_D0);
  dev_gpio_input(S_D1);
  dev_gpio_input(S_D2);
  dev_gpio_input(S_D3);
  dev_gpio_input(S_D4);
  dev_gpio_input(S_D5);
  dev_gpio_input(S_D6);
  dev_gpio_input(S_D7);
}

/* Set value of address lines */
static void
s3_set_a(int address)
{
  dev_gpio_set(S_A0, address & 0x01);
  dev_gpio_set(S_A1, address & 0x02);
  dev_gpio_set(S_A2, address & 0x04);
  dev_gpio_set(S_A3, address & 0x08);
  dev_gpio_set(S_A4, address & 0x10);
  dev_gpio_set(S_A5, address & 0x20);
  dev_gpio_set(S_A6, address & 0x40);
  dev_gpio_set(S_A7, address & 0x80);
}

/* Set value of data lines */
static void
s3_set_d(int value)
{
  dev_gpio_set(S_D0, value & 0x01);
  dev_gpio_set(S_D1, value & 0x02);
  dev_gpio_set(S_D2, value & 0x04);
  dev_gpio_set(S_D3, value & 0x08);
  dev_gpio_set(S_D4, value & 0x10);
  dev_gpio_set(S_D5, value & 0x20);
  dev_gpio_set(S_D6, value & 0x40);
  dev_gpio_set(S_D7, value & 0x80);
}

/* Read value of data lines */
static int
s3_get_d()
{
  int r= 0;

  r|= dev_gpio_get(S_D0) << 0;
  r|= dev_gpio_get(S_D1) << 1;
  r|= dev_gpio_get(S_D2) << 2;
  r|= dev_gpio_get(S_D3) << 3;
  r|= dev_gpio_get(S_D4) << 4;
  r|= dev_gpio_get(S_D5) << 5;
  r|= dev_gpio_get(S_D6) << 6;
  r|= dev_gpio_get(S_D7) << 7;

  return r;
}

/* Set RD line */
static void
s3_set_rd(int value)
{
  dev_gpio_set(S_RD, value);
}

/* Set WR line */
static void
s3_set_wr(int value)
{
  dev_gpio_set(S_WR, value);
}

/* Set RST line */
/*
static void
s3_set_rst(int value)
{
  dev_gpio_set(S_RST, value);
}
*/

/* Read value of addressed register */
int
s3_read(int address)
{
  int r;
  s3_d_off();           /* Setup data lines for READ */
  s3_set_a(address);    /* Set address lines */
  f4_delay(1);          /* Data setup time */
  s3_set_rd(1);         /* Pulse on RD */
  f4_delay(1);          /* Data hold time */
  r= s3_get_d();        /* Read value from data lines */
  s3_set_rd(0);         /* Release RD line */
  return r;             /* Return read value */
}

/* Write value to addressed register */
int
s3_write(int address, int value)
{
  s3_d_on();            /* Setup data lines for WRITE */
  s3_set_a(address);    /* Set address lines */
  s3_set_d(value);      /* Put value on data lines */
  f4_delay(1);          /* Data setup time */
  s3_set_wr(1);         /* Pulse on WR line */
  f4_delay(1);          /* Data hold time */
  s3_set_wr(0);         /* Release WR line */
  f4_delay(1);          /* Some more delay... */
  s3_d_off();           /* Release data lines */
  return value;         /* Return written value */
}

/*
static void
s3_printd(int v)
{
	char s[5];
	int i;
	snprintf(s, 5, "%-d", v);
	for (i= 0; (i<4) && (s[i]); i++)
	{
		if (s[i]==' ')
		{
			s3_write(11-i, 0);
		}
		else
		{
			s3_write(3-i, s[i]-'0');
		}
	}
	for ( ; i<4; i++)
		s3_write(11-i, 0);
}
*/

/* End of s3.c */
