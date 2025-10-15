#ifndef SERIAL_HEADER
#define SERIAL_HEADER


//#include "os-support.h"

#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
//#include <termio.h>
//#include <sys/termios.h>
//#include <sys/ioctl.h>
#include "wrap_termios.h"

#ifdef __cplusplus
extern "C" {
#endif

#define COM_PORT_SELECTABLE 1

  /*
    Modem Status Flags
  */
#define MS_CTS_ON			0x0010
#define MS_DSR_ON			0x0020
#define MS_RING_ON			0x0040
#define MS_RLSD_ON			0x0080


  enum s_interface {
    MSP_RS232_MODE,		/* 2 */
    MSP_RS485_2WIRE_MODE,	/* 5 */
    MSP_RS422_MODE,		/* 4 */
    MSP_RS485_4WIRE_MODE	/* 8 */
    };
  
  enum s_parity {
    MSP_PARITY_NONE,		/* N */
    MSP_PARITY_ODD,		/* O */
    MSP_PARITY_EVEN,		/* E */
    MSP_PARITY_SPACE,		/* S */
    MSP_PARITY_MARK,		/* M */
    MSP_PARITY_1,		/* 1 */
    MSP_PARITY_0		/* 0 */
  };
  
  enum s_flow {
    MSP_FLOWCTRL_NONE,		/* N */
    MSP_FLOWCTRL_SW,		/* S */
    MSP_FLOWCTRL_HW		/* H */
  };
  
  /* [2,]19200,N,8,1[,N] */
  struct s_params {
    enum s_interface interface;
    enum s_parity parity;
    enum s_flow flow;
    int br;
    int data_bits;
    int stop_bits;
  };

  enum {
    MSP_PURGE_RX,
    MSP_PURGE_TX,
    MSP_PURGE_RXTX,
  };

  extern int serial_parse_params(char *param_str, struct s_params *params);
  
  /*	serial device interface		*/
  //extern int mxsp_read(unsigned int fd, char *, int, void *);
  //extern int mxsp_write(unsigned int fd, char *, int, void *);
  
  extern unsigned int mxsp_open(int);
  extern int serial_open(char *fname,
			 struct s_params *params,
			 int timeout);
  extern int serial_popen(char *fname, char *paramstr);
  extern int serial_canopen(char *fname,
			    struct s_params *params,
			    int echo);
  extern int serial_pcanopen(char *fname, char *paramstr);
  extern void serial_echo(int fd, int echo);
  extern int mxsp_close(unsigned int fd);
  extern void mxsp_set_nonblocking(unsigned int fd);
  
  extern int mxsp_purge_buffer(unsigned int fd, int);
  
  /* set communication settings */
  extern int  mxsp_set_baudrate(unsigned int fd, int baudrate);
  extern void cf_setspeed(struct termios *tp, int speed);
  extern int  mxsp_set_databits(unsigned int fd, int bits);
  extern void cf_setdatabits(struct termios *tp, int databits);
  extern int  mxsp_set_stopbits(unsigned int fd, int bits);
  extern void cf_setstopbits(struct termios *tp, int stopbits);
  extern int  mxsp_set_parity(unsigned int fd, int parity);
  extern void cf_setparity(struct termios *tp, enum s_parity parity);
  //extern int  mxsp_set_interface(unsigned int fd, int );
  extern int  mxsp_set_flow_control(unsigned int fd, int mode);
  extern void cf_setflowcontrol(struct termios *tp, enum s_flow flow);
  extern int  mxsp_set_timeout_read(unsigned int fd, int );
  extern void cf_setsparams(struct termios *tp, char *paramstr);
  extern int  tc_setsparams(int fd, char *paramstr);

  /* get communication settings */
  extern int mxsp_get_baudrate(unsigned int fd);
  extern int mxsp_get_databits(unsigned int fd);
  extern int mxsp_get_stopbits(unsigned int fd);
  extern int mxsp_get_parity(unsigned int fd);
  //extern int mxsp_get_interface(unsigned int fd);
  extern int mxsp_get_flow_control(unsigned int fd);
  
  extern int mxsp_inqueue(unsigned int fd);
  extern int mxsp_outqueue(unsigned int fd);
  
  extern int mxsp_set_xonxoff(unsigned int fd ,char , char );
  extern int mxsp_set_break(unsigned int fd);
  extern int mxsp_clear_break(unsigned int fd);
  
#ifdef __cplusplus
}
#endif

#endif
