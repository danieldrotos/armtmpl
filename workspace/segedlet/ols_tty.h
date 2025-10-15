#ifndef OLS_TTY_HEADER
#define OLS_TTY_HEADER


#include "system_termios.h"


enum {
  tty_ibuf_size= 128,
  tty_obuf_size= 128,
  tty_lbuf_size= 100
};

struct tty_buf {
  int first_occupied;
  int first_free;
};

struct tty_line {
  int len;
  int cursor;
};

struct tty_t {
  struct termios termios;
  char ibuf[tty_ibuf_size];
  char obuf[tty_obuf_size];
  char line[tty_lbuf_size];
  struct tty_buf ib, ob;
  struct tty_line lb;
};


extern void tty_init(struct tty_t *tty);

extern int tty_received(struct tty_t *tty, char c, int line_status);
extern int tty_readable(struct tty_t *tty);
extern int tty_read(struct tty_t *tty, char *buf, int len);

extern int tty_write(struct tty_t *tty, char *buf, int len);
extern int tty_writable(struct tty_t *tty);
extern int tty_oget(struct tty_t *tty);


#endif
