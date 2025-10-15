#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>

#include "wrap_ioctl.h"
#include "wrap_termios.h"

//#include "uall.h"

#include "serial.h"


#define CMSPAR		010000000000

//#define MOXA_SET_OP_MODE                (0x400+66)
//#define MOXA_GET_OP_MODE                (0x400+67)
//#define UC_SET_SPECIAL_BAUD_RATE        (0x400+68)
//#define UC_GET_SPECIAL_BAUD_RATE        (0x400+69)
//#define MOXA_SET_SPECIAL_BAUD_RATE      (0x400+100)
//#define MOXA_GET_SPECIAL_BAUD_RATE      (0x400+101)


/* [2,]19200,N,8,1[,N]
   1         2 3 4  5
 */
int
serial_parse_params(char *param_str, struct s_params *params)
{
  char *s, *delims= ",;.|/ ", c, *p;
  int i, v;

  /* set defaults */
  params->interface= MSP_RS232_MODE;
  params->br= 19200;
  params->parity= MSP_PARITY_NONE;
  params->data_bits= 8;
  params->stop_bits= 1;
  params->flow= MSP_FLOWCTRL_NONE;

  i= 1;
  p= strdup(param_str);
  s= strtok(p, delims);
  while (s)
    {
      v= strtol(s, 0, 0);
      c= toupper((int)(*s));
      switch (i)
	{
	case 1:
	  if (v<100)
	    switch (c)
	      {
	      case '2': params->interface= MSP_RS232_MODE; break;
	      case '5': params->interface= MSP_RS485_2WIRE_MODE; break;
	      case '4': params->interface= MSP_RS422_MODE; break;
	      case '8': params->interface= MSP_RS485_4WIRE_MODE; break;
	      }
	  else
	    {
	      params->br= v;
	      i++;
	    }
	  break;
	case 2:
	  switch (c)
	    {
	    case 'N': params->parity= MSP_PARITY_NONE; break;
	    case 'O': params->parity= MSP_PARITY_ODD; break;
	    case 'E': params->parity= MSP_PARITY_EVEN; break;
	    case 'S': params->parity= MSP_PARITY_SPACE; break;
	    case 'M': params->parity= MSP_PARITY_MARK; break;
	    case '1': params->parity= MSP_PARITY_1; break;
	    case '0': params->parity= MSP_PARITY_0; break;
	    }
	  i++;
	  break;
	case 3:
	  params->data_bits= v;
	  i++;
	  break;
	case 4:
	  params->stop_bits= v;
	  i++;
	  break;
	case 5:
	  switch (c)
	    {
	    case 'N': params->flow= MSP_FLOWCTRL_NONE; break;
	    case 'S': params->flow= MSP_FLOWCTRL_SW; break;
	    case 'H': params->flow= MSP_FLOWCTRL_HW; break;
	    }
	  i++;
	  break;
	}
      s= strtok(NULL, delims);
    }
  free(p);
  return 0;
}


/* set the baudrate on an open port
	Inputs:
		<fd> handler of the open port
		<baudrate> a specified baudrate
	Returns:
		0 on success, otherwise failure
*/
/*	baud rate setting (including the extented rates that are not included 
	in the speed table 
	<return> 0 on success, failure otherwise 
*/
int	
mxsp_set_baudrate ( unsigned int fd, int baudrate)
{
  struct termios termios;

  /* check and setup configuration */
  if (tcgetattr(fd, &termios))
    return -1;
  
  cf_setspeed(&termios, baudrate);
  return tcsetattr(fd, TCSANOW, &termios)? -2:0;

  if (0)
    {
      /* MOXA special baud rate */
      termios.c_cflag |= B4000000;
      //return ioctl(fd, UC_SET_SPECIAL_BAUD_RATE, &baudrate)? -2:0;
    }
}

void
cf_setspeed(struct termios *tp, int speed)
{
  cfsetspeed(tp, speed);
}

/* get the baudrate on an open port
	Inputs:
		<fd> handler of the open port
	Returns:
		negative value on failure, otherwise the baudrate
*/
int	
mxsp_get_baudrate(unsigned int fd)
{
  struct termios termios;
  
  /* check and setup configuration */
  if (tcgetattr(fd, &termios))
    return -1;
  
  return cfgetospeed(&termios);
}

/* set the data bits on an open port
	Inputs:
		<fd> handler of the open port
		<bits> 5, 6, 7, or 8
	Returns:
		0 on success, otherwise failure
*/
int
mxsp_set_databits( unsigned int fd, int bits)
{
  struct termios	termios;

  if (tcgetattr(fd, &termios))
    return -1;

  cf_setdatabits(&termios, bits);

  return tcsetattr(fd, TCSANOW, &termios)? -2:0;
}

void
cf_setdatabits(struct termios *tp, int databits)
{
  if (tp==NULL)
    return;

  tp->c_cflag &= ~CSIZE;
  if (databits==5)	tp->c_cflag |= CS5;
  else if (databits==6)	tp->c_cflag |= CS6;
  else if (databits==7)	tp->c_cflag |= CS7;
  else			tp->c_cflag |= CS8;  
}


/* get the data bits on an open port
	Inputs:
		<fd> handler of the open port
	Returns:
		5~8 on success, otherwise failure
*/
int
mxsp_get_databits( unsigned int fd )
{
  unsigned int flag;
  struct termios	termios;
  
  if (tcgetattr(fd, &termios))
    return -1;
  flag = termios.c_cflag & CSIZE;
  if (flag == CS8)		return 8;
  else if (flag == CS7)	return 7;
  else if (flag == CS6)	return 6;
  else if (flag == CS5)	return 5;
  else 	return -99;
}

/* set the # of stop bits on an open port
	Inputs:
		<fd> handler of the open port
		<bits> stop bits, 1, 2, or 3 (for 1.5)
	Returns:
		0 on success, otherwise failure
*/
int
mxsp_set_stopbits( unsigned int fd, int bits)
{
  struct termios	termios;

  if (tcgetattr( fd, &termios))
    return -1;

  cf_setstopbits(&termios, bits);

  return tcsetattr(fd, TCSANOW, &termios)? -2:0;
}

void
cf_setstopbits(struct termios *tp, int stopbits)
{
  if (tp==NULL)
    return;

  if (stopbits == 2)
    tp->c_cflag|= CSTOPB;
  else
    tp->c_cflag&= ~CSTOPB;
}

/* get the # of stop bits of an open port
	Inputs:
		<fd> handler of the open port
	Returns:
		negative value on failure, or stop bits, 1, 2, and 3 (for 1.5)
*/
int
mxsp_get_stopbits( unsigned int fd)
{
  struct termios	termios;
  int		bits;

  /* check and setup configuration */
  if (tcgetattr(fd, &termios)==0)
    return -1;
  
  if (termios.c_cflag & CSTOPB)
    bits = 2;
  else
    bits = 1;
  
  return bits;
}

/* set the parity of an open port
	Inputs:
		<fd> handler of the open port
		<parity> parity code , 0:none, 1:odd, 2:even, 3:space, 4:mark, otherwise:none
	Returns:
		negative value on failure, or the parity code
*/
int
mxsp_set_parity( unsigned int fd, int parity)
{
  struct termios	termios;

  /* check and setup configuration */
  if (tcgetattr(fd, &termios))
    return -1;
  
  cf_setparity(&termios, parity);

  return (tcsetattr(fd, TCSANOW, &termios)) ? -2 : 0;
}

void
cf_setparity(struct termios *tp, enum s_parity parity)
{
  if (tp==NULL)
    return;

  tp->c_cflag &= ~(PARENB|PARODD|CMSPAR);
  if (parity == MSP_PARITY_ODD)		tp->c_cflag |= (PARENB|PARODD);
  else if (parity == MSP_PARITY_EVEN)	tp->c_cflag |= PARENB;
  else if (parity == MSP_PARITY_SPACE)	tp->c_cflag |= (PARENB|CMSPAR);
  else if (parity == MSP_PARITY_MARK)	tp->c_cflag |= (PARENB|CMSPAR|PARODD);
}

/* get the parity of an open port
	Inputs:
		<fd> handler of the open port
	Returns:
		negative value on failure, or the parity code
*/
int
mxsp_get_parity( unsigned int fd)
{
  int	parity;
  struct termios	termios;

  /* check and setup configuration */
  if (tcgetattr(fd, &termios))
    return -1;
  
  if ( termios.c_cflag & PARENB ) 
    {
      if ( termios.c_cflag & CMSPAR ) 
	parity = ( termios.c_cflag & PARODD )? MSP_PARITY_MARK:MSP_PARITY_SPACE;
      else 
	parity = ( termios.c_cflag & PARODD )? MSP_PARITY_ODD:MSP_PARITY_EVEN;
    } 
  else
    parity = MSP_PARITY_NONE;
  return parity;
}

/* set the flow control mode of an open port
	Inputs:
		<fd> handler of the open port
		<mode> 1:software, 2:hardware, otherwise:none
	Returns:
		0 on success, otherwise failure
*/
int
mxsp_set_flow_control( unsigned int fd, int mode)
{
  struct termios termios;

  if (tcgetattr(fd, &termios))
    return -1;
  
  cf_setflowcontrol(&termios, mode);

  return tcsetattr(fd, TCSANOW, &termios)? -1:0;
}

void
cf_setflowcontrol(struct termios *tp, enum s_flow flow)
{
  if (tp==NULL)
    return;

  if (flow == MSP_FLOWCTRL_NONE)	
    {
      tp->c_cflag &= ~CRTSCTS;
      tp->c_iflag &= ~(IXON | IXOFF | IXANY);
    }
  else if (flow == MSP_FLOWCTRL_HW)
    {
      tp->c_iflag &= ~(IXON | IXOFF | IXANY);
      tp->c_cflag |= CRTSCTS;
    }
  else if (flow == MSP_FLOWCTRL_SW)
    {
      tp->c_cflag &= ~CRTSCTS;
      tp->c_iflag |= (IXON|IXOFF|IXANY);
      /* added by Carl, which are the same as Windows */
      tp->c_cc[VSTART] = 0x11;    /* Ctrl-Q */
      tp->c_cc[VSTOP] = 0x13;     /* Ctrl-S */
    }
}

/* get the flow control mode of an open port
	Inputs:
		<fd> handler of the open port
	Returns:
		negative value on failure, or the flow control mode
*/
int	
mxsp_get_flow_control( unsigned int fd)
{
  struct termios termios;

  if (tcgetattr(fd, &termios))
    return -1;
  
  if ( termios.c_cflag & CRTSCTS )
    return MSP_FLOWCTRL_HW;
  else if ( termios.c_iflag & (IXON|IXOFF) )
    return MSP_FLOWCTRL_SW;
  else
    return MSP_FLOWCTRL_NONE;
}

/* set the operational interface of an open port
	Inputs:
		<fd> handler of the open port
		<mode> interface mode , 
	Returns:
		0 on success, otherwise failure
*/
/*int
mxsp_set_interface(unsigned int fd, int mode)
{
  if (mode < MSP_RS232_MODE || mode > MSP_RS485_4WIRE_MODE )
    return -99;
  if (ioctl(fd, MOXA_SET_OP_MODE, &mode) != 0)
    return -1;
  
  return 0;
}*/

/* get the operational interface of an open port
	Inputs:
		<fd> handler of the open port
	Returns:
		negative value on failure, otherwise the operational interface mode
*/
/*int
mxsp_get_interface(unsigned int fd)
{
  int mode;

  return ioctl(fd, MOXA_GET_OP_MODE, &mode)==0? mode:-1;
}*/

/* purge the buffers of an open port
	Inputs:
		<purge> 0: receive data buffer, 1: transmit data buffer, 2: both
	Returns:
		0 on success, otherwise failure
*/
int 
mxsp_purge_buffer( unsigned int fd, int purge)
{
  int mode;

  /* no overlapped no MSP_PURGE_RXABORT or MSP_PURGE_TXABORT */
  if ( purge == MSP_PURGE_RX)		mode = TCIFLUSH;
  else if (purge == MSP_PURGE_TX) mode = TCOFLUSH;
  else if (purge == MSP_PURGE_RXTX) mode = TCIOFLUSH;
  else
    return -99;
  
  return (tcflush(fd, mode))? -2:0;
}

/* set the open port to non-blocking reading
	Inputs:
		<fd> the open port
*/
void
mxsp_set_nonblocking(unsigned int fd)
{
  fcntl(fd, F_SETFL, FNDELAY);
#if 0
  fcntl(fd, F_SETFL, 0); // resume to blocking mode
#endif
}

/* close a serial port by the port number, not the name 
	Inputs:
		<port> port number
	Returns:
		0 on success, otherwise a negative value
*/
int
mxsp_close( unsigned int fd )
{
  close(fd);
  return 0;
}

/* open a serial port by the port number, not the name 
	Inputs:
		<port> port number
	Returns:
		0 on failure, otherwise the handle of an open port
	Notes:
		the default communication setting: RS232/HW Flow Control/N/8/1/9600
*/
unsigned int
mxsp_open(int port)
{
  int	fd;
  char	ttyname[64];

  sprintf(ttyname, "/dev/ttyM%d", port-1);
  fd = open(ttyname, O_RDWR|O_NOCTTY);
  if ( fd <= 0 ) 
    {
      //debug("fail to open serial port %s",ttyname);
      return 0;
    }
  else
    {
      struct termios	tmio;
      
      tcflush(fd, TCIOFLUSH);
      
      tcgetattr(fd, &tmio);
      tmio.c_lflag = 0;
      tmio.c_oflag = 0;
      tmio.c_iflag = 0;
      tmio.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
      tmio.c_cc[VTIME] = 0;
      tmio.c_cc[VMIN] = 0;
      tcsetattr(fd, TCSANOW, &tmio);
      
      //mxsp_set_interface((unsigned int) fd, MSP_RS232_MODE);
      return (unsigned int) fd;
    }
}


void
cf_setsparams(struct termios *tp, char *paramstr)
{
  struct s_params params;
  
  serial_parse_params(paramstr, &params);

  cf_setspeed(tp, params.br);
  cf_setparity(tp, params.parity);
  cf_setdatabits(tp, params.data_bits);
  cf_setstopbits(tp, params.stop_bits);
  cf_setflowcontrol(tp, params.flow);
}

int
tc_setsparams(int fd, char *paramstr)
{
  struct termios tp;

  tcgetattr(fd, &tp);
  cf_setsparams(&tp, paramstr);
  return tcsetattr(fd, TCSANOW, &tp);
}

int
serial_open(char *fname,
	    struct s_params *params,
	    //int br,
	    //int parity,
	    //int data_bits,
	    //int stop_bits,
	    //int flow,
	    int timeout/*,
			 int mode*/)
{
  int fd= open(fname,O_RDWR|O_NOCTTY);//mxsp_open(atoi(argv[1]));
  if (fd<=0)
    fprintf(stderr, "Open: %s", fname);
  else
    {
      struct termios	tmio;

      if (tcflush(fd, TCIOFLUSH)<0)
	fprintf(stderr, "tcflush: %s\n", strerror(errno));
      
      if (tcgetattr(fd, &tmio)<0)
	fprintf(stderr, "tcgetattr: %s\n", strerror(errno));
      tmio.c_lflag = 0;
      tmio.c_oflag = 0;
      tmio.c_iflag = 0;
      tmio.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
      tmio.c_cc[VTIME] = timeout;
      tmio.c_cc[VMIN] = 0;
      
      cf_setspeed(&tmio, params->br);
      cf_setparity(&tmio, params->parity);
      cf_setdatabits(&tmio, params->data_bits);
      cf_setstopbits(&tmio, params->stop_bits);
      cf_setflowcontrol(&tmio, params->flow);
      
      if (tcsetattr(fd, TCSANOW, &tmio)<0)
	fprintf(stderr, "tcsetattr");

	/*  if (mxsp_set_interface(fd, params->interface)<0)
	fprintf(stderr, "set_interface");*/
    }
  return fd;
}

int
serial_popen(char *fname, char *paramstr)
{
  struct s_params params;
  serial_parse_params(paramstr, &params);
  return serial_open(fname, &params, 0);
}

int
serial_canopen(char *fname,
	       struct s_params *params,
	       int echo)
{
  int fd= open(fname,O_RDWR|O_NOCTTY);//mxsp_open(atoi(argv[1]));
  if (fd<=0)
    fprintf(stderr, "Open: %s", fname);
  else
    {
      struct termios	tmio;

      if (tcflush(fd, TCIOFLUSH)<0)
	fprintf(stderr, "tcflush: %s\n", strerror(errno));
      
      if (tcgetattr(fd, &tmio)<0)
	fprintf(stderr, "tcgetattr: %s\n", strerror(errno));
      tmio.c_lflag = ICANON | (echo?ECHO:0);
      tmio.c_oflag = 0;
      tmio.c_iflag = IGNCR;
      tmio.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
      tmio.c_cc[VTIME] = 0;
      tmio.c_cc[VMIN] = 0;
      
      cf_setspeed(&tmio, params->br);
      cf_setparity(&tmio, params->parity);
      cf_setdatabits(&tmio, params->data_bits);
      cf_setstopbits(&tmio, params->stop_bits);
      cf_setflowcontrol(&tmio, params->flow);
      
      if (tcsetattr(fd, TCSANOW, &tmio)<0)
	fprintf(stderr, "tcsetattr");

    /*  if (mxsp_set_interface(fd, params->interface)<0)
	fprintf(stderr, "set_interface");*/
    }
  return fd;
}

int
serial_pcanopen(char *fname, char *paramstr)
{
  struct s_params params;
  serial_parse_params(paramstr, &params);
  return serial_canopen(fname, &params, 0);
}


void
serial_echo(int fd, int echo)
{
  struct termios tmio;
  tcgetattr(fd, &tmio);
  if (echo)
    tmio.c_lflag|= ECHO;
  else
    tmio.c_lflag&= ~ECHO;
  tcsetattr(fd, TCSANOW, &tmio);
}

/* the number of bytes received by the serial. 
	Inputs:
		<fd> the open port
	Returns:
		< 0 on failure, the number of bytes
*/
int	
mxsp_inqueue(unsigned int fd)
{
  long bytes;

  if ( ioctl(fd, TIOCINQ, &bytes) != 0)
    return -1;
  return (int) bytes;
}

/* the number of bytes user data remaining to be transmitted
	Inputs:
		<fd> the open port
	Returns:
		< 0 on failure, the number of bytes
*/
int
mxsp_outqueue( unsigned int fd )
{
  long bytes;

  if ( ioctl(fd, TIOCOUTQ, &bytes) != 0)
    return -1;
  return (int) bytes;
}

/* byte to byte timeout  = ReadIntervalTimeout */
/* read operation timeout  = ReadTotalTimeoutConstant + ReadTotalTimeoutMultiplier x (bytes to read) */
/* set the time-out for read operations on the open port
	Inputs:
		<fd> handler of the open port
		<mSec> time-out, in milliseconds
*/
int 
mxsp_set_timeout_read(unsigned int fd, int mSec)
{
  int sec;
  struct termios	termios;

  /* check and setup configuration */
  if (tcgetattr(fd, &termios))
    return -1;
  
  sec = mSec / 100;
  termios.c_cc[VTIME] = (sec > 1) ? sec : 1;
  termios.c_cc[VMIN] = 0;
  
  return tcsetattr(fd, TCSANOW, &termios)? -1:0;
}

/* write operation timeout = WriteTotalTimeoutConstant + WriteTotalTimeoutMultiplier x (bytes to write) */
/* set the time-out for write operations on the open port
	Inputs:
		<fd> handler of the open port
		<mSec> time-out, in milliseconds
*/
/*
static int 
mxsp_set_timeout_write(unsigned int fd, int mSec)
{
  (void) fd;
  (void) mSec;
  return 0;
}
*/

/* write data to the open port
	Inputs:
		<fd> handler of the open port
		<buffer> point to the buffer
		<size> size of the data to be written
		<dummy> reserved argument
	Returns:
		< 0 on failure, otherwise the number of bytes written
*/
/*int
mxsp_write( unsigned int fd, char *buffer, int size, void *dummy)
{
  (void) dummy;
    
  return write(fd, buffer, size);
}*/

/* read data onto a buffer from an open port (nonblocking mode)
	Inputs:
		<fd> handler of the open port
		<buffer> point to the buffer
		<size> maximum size to be read
		<dummy> reserved argument
	Returns:
		< 0, on failure
		0, no data ready
		otherwise the number of bytes read
*/
/*int
mxsp_read( unsigned int fd, char *buffer, int size, void *dummy)
{
  (void) dummy;

  return read(fd, buffer, size);
}*/

/* set the xon/xoff characters for software flow control
	Inputs:
		<fd> the open port
		<xon> xon character
		<xoff> xoff character
	Returns:
		0 on success, otherwise failure
*/
int 
mxsp_set_xonxoff( unsigned int fd ,char xon, char xoff)
{
  struct termios	termios;

  /* check and setup configuration */
  if (tcgetattr(fd, &termios))
    return -1;
  
  termios.c_cc[VSTART] = xon;
  termios.c_cc[VSTOP] = xoff;
  
  return tcsetattr(fd, TCSANOW, &termios)? -1:0;
}

int
mxsp_get_errors(unsigned int fd, unsigned int *err)
{
  
  return 0;
}

int	
mxsp_set_break(unsigned int fd)
{
  return (ioctl(fd, TIOCSBRK, NULL)? -1:0);
}

int	
mxsp_clear_break(unsigned int fd)
{
  return (ioctl(fd, TIOCCBRK, NULL)? -1:0);
}

/* get the modem status (line status)
	Inputs:
		<fd> the open port
	Outputs:
		<lstatus> modem status in windows definitions
			TIOCM_CAR -> MS_RLSD_ON
			TIOCM_RNG -> MS_RING_ON
			TIOCM_DSR -> MS_DSR_ON
			TIOCM_CTS -> MS_CTS_ON
	Returns:
		0 on success, otherwise failure
*/
int
mxsp_get_lstatus(unsigned int fd, unsigned int *lstatus)
{
  int	status;
  
  *lstatus = 0;
  if ( ioctl(fd, TIOCMGET, &status) )
    return -1;
  
  if ( status & TIOCM_CAR )	*lstatus |= MS_RLSD_ON;
  if ( status & TIOCM_RNG )	*lstatus |= MS_RING_ON;
  if ( status & TIOCM_DSR )	*lstatus |= MS_DSR_ON;
  if ( status & TIOCM_CTS )	*lstatus |= MS_CTS_ON;
  return 0;
}

#if 0
int
mxsp_set_lctrl(unsigned int fd, unsigned int lst)
{
  int		ctrl=0;
  struct termios	tmio;
  
  if (tcgetattr(fd, &tmio))
    return -1;
  if ( mode & C_RTS ) 
    {
      if ( tmio.c_cflag & CRTSCTS )
	return 0;
      ctrl |= TIOCM_RTS;
    }
}

int	sio_lctrl(int port, int mode)
{
  if ( mode & C_RTS ) {
    if ( serial->termios.c_cflag & CRTSCTS )
      return SIO_RTS_BY_HW;
    ctrl |= TIOCM_RTS;
  }
  if ( mode & C_DTR )
    ctrl |= TIOCM_DTR;
  if ( ioctl(serial->fd, TIOCMSET, &ctrl) )
    return SIO_FAIL;
  return SIO_OK;
}
#endif
