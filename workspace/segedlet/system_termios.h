#ifndef SYSTEM_TERMIOS_HEADER
#define SYSTEM_TERMIOS_HEADER

typedef unsigned int tcflag_t;
typedef unsigned int cc_t;
typedef unsigned int speed_t;

enum termio_iflags {
  INPCK		= 0x00000001,	// 1=enable input parity check
  IGNPAR	= 0x00000002,	// 1=ignore bytes with frame/parity error
  PARMRK	= 0x00000004,	// 1=mark frame/parity error bytes by 0377 0
  ISTRIP	= 0x00000008,	// 1=strip input bytes to 7 bit
  IGNBRK	= 0x00000010,	// 1=ignore break conditions
  BRKINT	= 0x00000020,	// 1=break condition interrupts program
  IGNCR		= 0x00000040,	// 1=discard CR (\r)
  ICRNL		= 0x00000080,	// 1=change CR (\r) to NL (\n)
  INLCR		= 0x00000100,	// 1=change NL (\n) to CR (\r)
  IXOFF		= 0x00000200,	// 1=enable START/STOP control on input
  IXON		= 0x00000400,	// 1=enable START/STOP control on output
  IXANY		= 0x00000800,	// 1=any input restarts output
  IMAXBEL	= 0x00001000	// 1=input buffer full sends BELL to terminal
};

enum termio_oflags {
  OPOST		= 0x00000001,	// 1=output data is processed
  ONLCR		= 0x00000002,	// 1=convert NL (\n) to CRLF
  OXTABS	= 0x00000004,	// 1=convert TAB to spaces
  ONOEOT	= 0x00000008	// 1=discard C-d (004) on output
};

enum termio_cflags {
  CLOCAL	= 0x00000001,	// 1=local terminal, discard modem status lines
  HUPCL		= 0x00000002,	// 1=generate modem disconnect when processe ends
  CREAD		= 0x00000004,	// 1=enable input
  CSTOPB	= 0x00000008,	// 1=use 2 stop bits
  PARENB	= 0x00000010,	// 1=parity check enabled
  PARODD	= 0x00000020,	// 1=odd parity
  CSIZE		= 0x000001c0,	// mask of bits/char
  CS5		= 0x00000040,	// 5 bits/char
  CS6		= 0x00000080,	// 6 bits/char
  CS7		= 0x000000c0,	// 7 bits/char
  CS8		= 0x00000100,	// 8 bits/char
  CBAUDEX	= 0x00000200,	// 1=extra baud rate
  CBAUD		= 0x0001f000,	// bitmask for standard baud rates
  /* BSD extensions: */
  CCTS_OFLOW	= 0x00020000,	// 1=output flow control by CTS
  CTRS_IFLOW	= 0x00040000,	// 1=output flow control by RTS
  MDMBUF	= 0x00080000,	// 1=output flow control by carrier
  CIGNORE	= 0x00100000,	// 1=tcsetattr ignores hw settings (control, speed)
  /* Linux ext? */
  CRTSCTS	= 0x00200000	// ?
};

enum termio_lflags {
  ICANON	= 0x00000001,	// 1=canonical 0=non-canonical
  ECHO		= 0x00000002,	// 1=input is echoed back
  ECHOE		= 0x00000004,	// 1=echo ERASE
  ECHOPRT	= 0x00000008,	// 1=echo multi ERASE as \erased chars/
  ECHOK		= 0x00000010,	// 1=move to new lina after KILL
  ECHOKE	= 0x00000020,	// 1=display of KILL by erase entire line
  ECHONL	= 0x00000040,	// 1=(and ICANON=1) echo NL (\n') even ECHO=0
  ECHOCTL	= 0x00000080,	// 1=(and ECHO=1) echo ctrl chars as ^-X
  ISIG		= 0x00000100,	// 1=process INTR,QUIT,SUSP of input
  IEXTEN	= 0x00000200,	// 1=enable LNEXT,DISCARD characters
  NOFLSH	= 0x00000400,	// 1=INTR,QUIT,SUSP do not clear in/out queues
  TOSTOP	= 0x00000800,	// 1=writing process generates SIGTTOU
  ALTWERASE	= 0x00001000,	// 1=word begin is alnum+_
  FLUSHO	= 0x00002000,	// 1=discard output, toggled by DISCARD char input
  NOKERNINFO	= 0x00004000,	// 1=do not handle STATUS char
  PENDIN	= 0x00008000	// 1=line of input needs to be reprinted
};

enum termio_speeds {
  B0		= 0x00000000,
  B50		= 0x00001000,
  B75		= 0x00002000,
  B110		= 0x00003000,
  B134		= 0x00004000,
  B150		= 0x00005000,
  B200		= 0x00006000,
  B300		= 0x00007000,
  B600		= 0x00008000,
  B1200		= 0x00009000,
  B1800		= 0x0000a000,
  B2400		= 0x0000b000,
  B4800		= 0x0000c000,
  B9600		= 0x0000d000,
  B19200	= 0x0000e000,
  B38400	= 0x0000f000,
  B57600	= 0x00010000,
  B115200	= 0x00011000,
  B230400	= 0x00012000,
  B460800	= 0x00013000,
  B500000	= 0x00014000,
  B576000	= 0x00015000,
  B921600	= 0x00016000,
  B1000000	= 0x00017000,
  B1152000	= 0x00018000,
  B1500000	= 0x00019000,
  B2000000	= 0x0001a000,
  B2500000	= 0x0001b000,
  B3000000	= 0x0001c000,
  B3500000	= 0x0001d000,
  B4000000	= 0x0001e000,

  /* BSD */
  EXTA		= B19200, /* alias for B19200 */
  EXTB		= B38400 /* alias for B38400 */
};

enum termio_chars {
  VEOF		= 0, /* C-d 4 */
  VEOL		= 1, 
  VEOL2		= 2,
  VERASE	= 3, /* DEL 127 */
  VWERASE	= 4, /* C-w 23 */ /* BSD */
  VKILL		= 5, /* C-u 21 */
  VREPRINT	= 6, /* C-r 18 */ /* BSD */
  
  VINTR		= 7, /* C-c 3 */
  VQUIT		= 8, /* C-\ 28 */
  VSUSP		= 9, /* C-z 26 */
  VDSUSP	= 10, /* C-y 25 */

  VSTART	= 11, /* C-q 17 */
  VSTOP		= 12, /* C-s 19 */

  /* BSD */
  VLNEXT	= 13, /* C-v 22 */
  VDISCARD	= 14,
  VSTATUS	= 15,

  VMIN		= 16,
  VTIME		= 17,

  NCCS		= 18
};

#define _POSIX_VDISABLE '\0'
/* Compare a character C to a value VAL from the `c_cc' array in a
   `struct termios'.  If VAL is _POSIX_VDISABLE, no character can match it.  */
# define CCEQ(val, c)   ((c) == (val) && (val) != _POSIX_VDISABLE)

struct termios {
  tcflag_t c_iflag;
  tcflag_t c_oflag;
  tcflag_t c_cflag;
  tcflag_t c_lflag;
  cc_t c_cc[NCCS];
  speed_t c_speed;
};

enum termio_when {
  TCSANOW,
  TCSADRAIN,
  TCSAFLUSH,
  TCSASOFT
};

enum termio_queue {
  TCIFLUSH,
  TCOFLUSH,
  TCIOFLUSH
};

enum termio_action {
  TCOOFF,
  TCOON,
  TCIOFF,
  TCION
};

enum termio_mstatus {
  TIOCM_CAR= 1,
  TIOCM_RNG= 2,
  TIOCM_DSR= 4,
  TIOCM_CTS= 8
};

extern int tcgetattr(int fd, struct termios *tp);
extern int tcsetattr(int fd, int WHEN, struct termios *tp);

extern int isatty(int fd);
extern char *ttyname(int fd);
//extern int ttyname_r(int fd, char *buf, int len);

extern speed_t cfgetospeed(const struct termios *tp);
extern speed_t cfgetispeed(const struct termios *tp);
extern int cfsetospeed(struct termios *tp, speed_t speed);
extern int cfsetispeed(struct termios *tp, speed_t speed);
extern int cfsetspeed(struct termios *tp, speed_t speed);
extern void cfmakeraw(struct termios *tp);

extern int tcsendbreak(int fd, int duration);
extern int tcdrain(int fd);
extern int tcflush(int fd, int queue);
extern int tcflow(int fd, int action);


/* Defaults, originaly in sys/ttydefaults.h */

/*
 * Defaults on "first" open.
 */
#define TTYDEF_IFLAG    (BRKINT | ISTRIP | ICRNL | IMAXBEL | IXON | IXANY)
#define TTYDEF_OFLAG    (OPOST | ONLCR | XTABS)
#define TTYDEF_LFLAG    (ECHO | ICANON | ISIG | IEXTEN | ECHOE|ECHOKE|ECHOCTL)
#define TTYDEF_CFLAG    (CREAD | CS7 | PARENB | HUPCL)
#define TTYDEF_SPEED    (B9600)

/*
 * Control Character Defaults
 */
#define CTRL(x) (x&037)
#define CEOF            CTRL('d')
#ifdef _POSIX_VDISABLE
# define CEOL           _POSIX_VDISABLE
#else
# define CEOL           '\0'            /* XXX avoid _POSIX_VDISABLE */
#endif
#define CERASE          0177
#define CINTR           CTRL('c')
#ifdef _POSIX_VDISABLE
# define CSTATUS        _POSIX_VDISABLE
#else
# define CSTATUS        '\0'            /* XXX avoid _POSIX_VDISABLE */
#endif
#define CKILL           CTRL('u')
#define CMIN            1
#define CQUIT           034             /* FS, ^\ */
#define CSUSP           CTRL('z')
#define CTIME           0
#define CDSUSP          CTRL('y')
#define CSTART          CTRL('q')
#define CSTOP           CTRL('s')
#define CLNEXT          CTRL('v')
#define CDISCARD        CTRL('o')
#define CWERASE         CTRL('w')
#define CREPRINT        CTRL('r')
#define CEOT            CEOF
/* compat */
#define CBRK            CEOL
#define CRPRNT          CREPRINT
#define CFLUSH          CDISCARD

#endif
