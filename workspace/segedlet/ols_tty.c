#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "wrap_ioctl.h"

#include "ols_tty.h"


/* Init structure */

void
tty_init(struct tty_t *tty)
{
  if (!tty)
    return;
  memset(tty, 0, sizeof(struct tty_t));
  cfsetspeed(&(tty->termios), 115200);
  cfmakeraw(&(tty->termios));
  /*
cc_t    ttydefchars[NCCS] = {
        VINTR CEOF,
	VQUIT CEOL,
	VERASE CEOL,
	VKILL CERASE,
	VEOF CWERASE,
	VTIME CKILL,
	VMIN CREPRINT,
        VSWTC _POSIX_VDISABLE,
	VSTART CINTR,
	VSTOP CQUIT,
	VSUSP CSUSP,
	VEOL CDSUSP,
	VREPRINT CSTART,
	VDISCARD CSTOP,
	VLNEXT CLNEXT,
	VEOL2 CDISCARD,
	CMIN,
	CTIME,
	CSTATUS,
	_POSIX_VDISABLE
};
  */
  tty->termios.c_cc[VEOF]	= CEOF;		//  4; 0004 CTRL('d')
  tty->termios.c_cc[VERASE]	= CERASE;	//127; 0177
  tty->termios.c_cc[VKILL]	= CKILL;	// 21; 0025 CTRL('u')
  tty->termios.c_cc[VINTR]	= CINTR;	//  3; 0003 CTRL('c')
  tty->termios.c_cc[VQUIT]	= CQUIT;	// 28; 0034 CTRL('\')
  /* When ISIG set */
  tty->termios.c_cc[VSUSP]	= CSUSP;	// 26; 0032 CTRL('z')
  /* When IXON set */
  tty->termios.c_cc[VSTART]	= CSTART;	// 17; 0021 CTRL('q')
  tty->termios.c_cc[VSTOP]	= CSTOP;	// 19; 0023 CTRL('s')
  /* When IEXTEN set */
  tty->termios.c_cc[VREPRINT]	= CREPRINT;	// 18; 0022 CTRL('r')
  tty->termios.c_cc[VWERASE]	= CWERASE;	// 23; 0027 CTRL('w')
  tty->termios.c_cc[VLNEXT]	= CTRL('v');	// 22; 0026 CTRL('v') 
  tty->termios.c_cc[VDISCARD]	= CTRL('o');	// 15; 0017 CTRL('o')
  tty->termios.c_cc[VEOL2]	= _POSIX_VDISABLE;
  /* When IEXTEN ans ISIG set */
  tty->termios.c_cc[VDSUSP]	= CDSUSP;	// 25; 0031 CTRL('y')
}


/* Buffer handling */

static int
put(char *buf, struct tty_buf *b, int size, char c)
{
  int next= (b->first_free + 1) % size;
  if (next == b->first_occupied)
    return -1;
  buf[b->first_free]= c;
  b->first_free= next;
  return 0;
}

static int
iput(struct tty_t *tty, char c)
{
  return put(tty->ibuf, &(tty->ib), tty_ibuf_size, c);
}

static int
oput(struct tty_t *tty, char c)
{
  return put(tty->obuf, &(tty->ob), tty_obuf_size, c);
}

static int
oprint(struct tty_t *tty, char *s)
{
  int r= 0;
  while (!r &&
	 *s)
    {
      r= oput(tty, *s);
      s++;
    }
  return r;
}

static int
echo(struct tty_t *tty, char c)
{
  if (c > 31)
    return oput(tty, c);
  oput(tty, '^');
  return oput(tty, c + 64);
}

/* Editor */

static int
editor(struct tty_t *tty, char c)
{
  int lf= tty->termios.c_lflag;
  char *l= tty->line;
  cc_t *cc= &(tty->termios.c_cc[0]);
  struct tty_line *lb= &(tty->lb);
  int i;
  
  if (lb->len >= tty_lbuf_size - 1)
    return -1;
  
  if (CCEQ(cc[VEOL], c) ||
      CCEQ(cc[VEOL2], c) ||
      (c == '\n'))
    {
      for (i= 0; i < lb->len; i++)
	iput(tty, l[i]);
      iput(tty, c);
      lb->len= lb->cursor= l[0]= 0;
      if (lf & ECHO)
	oput(tty, c);
      return 0;
    }
  else if (CCEQ(cc[VERASE], c))
    {
      char erased;
      if (lb->cursor == 0)
	return 0;
      erased= l[lb->cursor-1];
      for (i= lb->cursor; i <= lb->len; i++)
	l[i-1]= l[i];
      lb->cursor--;
      lb->len--;
      if (lf & ECHO)
	{
	  if (lb->cursor == lb->len)
	    {
	      oprint(tty, "\010 \010");
	      if (erased < 32)
		oprint(tty, "\010 \010");
	    }
	  else
	    {
	      if (erased < 32)
		oprint(tty, "\033[2D");
	      else
		oprint(tty, "\033[1D");
	      oprint(tty, "\033[s");
	      for (i= lb->cursor; i < lb->len; i++)
		echo(tty, l[i]);
	      if (erased < 32)
		oprint(tty, "  ");
	      else
		oput(tty, ' ');
	      oprint(tty, "\033[u");
	    }
	}
      return 0;
    }
  else if (CCEQ(cc[VKILL], c))
    {
      lb->len= lb->cursor= l[0]= 0;
      if (lf & ECHO)
	oprint(tty, "\033[1G\033[2K");
      return 0;
    }
  else
    {
      if (lb->len == lb->cursor)
	{
	  l[lb->cursor++]= c;
	  lb->len++;
	}
      else
	{
	  for (i= lb->len; i >= lb->cursor; i++)
	    l[i+1]= l[i];
	  l[lb->cursor++]= c;
	  lb->len++;
	}
    }
  if (lf & ECHO)
    echo(tty, c);
  return 0;
}


/* Called from line driver's receive ISR */

int
tty_received(struct tty_t *tty, char c, int line_status)
{
  struct termios *t;
  int e, i, /*o,*/ l= line_status;
  
  if (!tty)
    return -1;
  t= &(tty->termios);
  i= t->c_iflag;
  //o= t->c_oflag;
  l= t->c_lflag;
  e= l & ECHO;
  
  switch (c)
    {
    case '\n':
      if (i & INLCR)
	c= '\r';
      break;
    case '\r':
      if (i & IGNCR)
	return 0;
      if (i & ICRNL)
	c= '\n';
      break;
    }
  if (i & ISTRIP)
    c&= 0x7f;

  if (l & ICANON)
    {
      editor(tty, c);
    }
  else
    {
      iput(tty, c);
      if (e)
	{
	  echo(tty, c);
	}
    }
  return 0;
}


/* Read */

int
tty_readable(struct tty_t *tty)
{
  if (!tty)
    return -1;
  if (tty->termios.c_lflag & ICANON)
    {
    }
  else
    {
    }
  return (tty->ib.first_free != tty->ib.first_occupied)?1:0;
}
	     
int
tty_read(struct tty_t *tty, char *buf, int len)
{
  struct tty_buf *ib;
  int i;
  
  if (!tty)
    return -1;
  i= tty_readable(tty);
  if (i < 0)
    return -2;
  if (i == 0)
    return 0;
  ib= &(tty->ib);
  i= 0;
  while ((ib->first_free != ib->first_occupied) &&
	 (i < len))
    {
      buf[i]= tty->ibuf[ib->first_occupied];
      i++;
      ib->first_occupied++;
      ib->first_occupied%= tty_ibuf_size;
    }
  return i;
}


/* Write */

int
tty_write(struct tty_t *tty, char *buf, int len)
{
  int i, r= 0;
  if (!tty)
    return -1;
  for (i= 0; (i < len) && (r == 0); i++)
    r= oput(tty, buf[i]);
  return i;
}

int
tty_writable(struct tty_t *tty)
{
  int n;
  if (!tty)
    return 0;
  n= (tty->ob.first_free + 1) % tty_obuf_size;
  return (n != tty->ob.first_occupied)?1:0;
}

int
tty_oget(struct tty_t *tty)
{
  struct tty_buf *ob;
  int n, c;
  if (!tty)
    return -1;
  ob= &(tty->ob);
  if (ob->first_free == ob->first_occupied)
    return -2;
  n= (tty->ob.first_occupied + 1) % tty_obuf_size;
  c= tty->obuf[ob->first_occupied];
  ob->first_occupied= n;
  return c;
}

   
/* TERMIOS interface */

struct termio_speed_table {
  enum termio_speeds speed;
  int br;
};

static const struct termio_speed_table termio_speed_table[]= {
  { B0		, 0 },
  { B50		, 50 },
  { B75		, 75 },
  { B110	, 110 },
  { B134	, 134 },
  { B150	, 150 },
  { B200	, 200 },
  { B300	, 300 },
  { B600	, 600 },
  { B1200	, 1200 },
  { B1800	, 1800 },
  { B2400	, 2400 },
  { B4800	, 4800 },
  { B9600	, 9600 },
  { B19200	, 19200 },
  { B38400	, 38400 },
  { B57600	, 57600 },
  { B115200	, 115200 },
  { B230400	, 230400 },
  { B460800	, 460800 },
  { B500000	, 500000 },
  { B576000	, 576000 },
  { B921600	, 921600 },
  { B1000000	, 1000000 },
  { B1152000	, 1152000 },
  { B1500000	, 1500000 },
  { B2000000	, 2000000 },
  { B2500000	, 2500000 },
  { B3000000	, 3000000 },
  { B3500000	, 3500000 },
  { B4000000	, 4000000 },

  { B0		, -1 }
};

speed_t
cfgetospeed(const struct termios *tp)
{
  return tp->c_speed;
}

speed_t
cfgetispeed(const struct termios *tp)
{
  return tp->c_speed;
}

int
cfsetospeed(struct termios *tp, speed_t speed)
{
  return cfsetspeed(tp, speed);
}

int
cfsetispeed(struct termios *tp, speed_t speed)
{
  return cfsetspeed(tp, speed);
}

int
cfsetspeed(struct termios *tp, speed_t speed)
{
  int i;
  
  tp->c_cflag|= CBAUDEX;
  tp->c_cflag&= ~CBAUD;
  tp->c_speed= speed;
  for (i= 0; termio_speed_table[i].br >= 0; i++)
    {
      if (termio_speed_table[i].br == (int)speed)
	{
	  tp->c_cflag&= ~CBAUDEX;
	  tp->c_cflag|= termio_speed_table[i].speed;
	  return 0;
	}
    }
  return 0;
}

void
cfmakeraw(struct termios *tp)
{
  tp->c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP
		   |INLCR|IGNCR|ICRNL|IXON);
  tp->c_oflag &= ~OPOST;
  tp->c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
  tp->c_cflag &= ~(CSIZE|PARENB);
  tp->c_cflag |= CS8;
}


int
tcgetattr(int fd, struct termios *tp)
{
  ioctl(fd, DIOGTTY, tp);
  return 0;
}

int
tcsetattr(int fd, int WHEN, struct termios *tp)
{
	ioctl(fd, DIOSTTY, tp);
	return 0;
}

int
tcsendbreak(int fd, int duration)
{
  return 0;
}

int
tcdrain(int fd)
{
  return 0;
}

int
tcflush(int fd, int queue)
{
  return 0;
}

int
tcflow(int fd, int action)
{
  return 0;
}

char *
ttyname(int fd)
{
  return "tty";//ols_ttyname(fd);
}

int
ttyname_r(int fd, char *buf, int len)
{
	strncpy(buf, "tty", 4);
  return 1;//errno= ols_ttyname_r(fd, buf, len);
}
