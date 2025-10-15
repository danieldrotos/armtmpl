#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>

#include "wrap_termios.h"
#include "wrap_ioctl.h"

#include "ols_driver.h"
#include "ols.h"
#include "ols_tty.h"

#include "dev_gpio.h"
#include "dev_usart.h"

//UART_TypeDef *u6= USART6;
//UART_TypeDef *u1= USART1;

#define EN_TC __HAL_UART_ENABLE_IT(&(dev->h), UART_IT_TC)
#define DI_TC __HAL_UART_DISABLE_IT(&(dev->h), UART_IT_TC)
#define EN_RX __HAL_UART_ENABLE_IT(&(dev->h), UART_IT_RXNE)
#define DI_RX __HAL_UART_DISABLE_IT(&(dev->h), UART_IT_RXNE)

struct dev_usart_device_t dev_usart_devices[USART_NUOF_DEVICES]= {
  /* 0 */
  {
    .h= {.Instance= NULL}
  },
  /* 1 */
  {
    .h= {.Instance=
#ifdef USART1
    		USART1
#else
			NULL
#endif
    },
    .tx_pin= "PA9", /* PA9,PB6 */
    .rx_pin= "PA10", /* PA10,PB7 */
    .cts_pin= NULL,
    .rts_pin= NULL,
    .gpio_af
#ifdef USART1
    		=GPIO_AF_USART1
#endif
			,
    .IRQn
#ifdef USART1
	= USART1_IRQn
#endif
	,
    .tx_buffer= {
      .buf= NULL
    },
    .rx_buffer= {
      .buf= NULL
    },
    .tty= NULL,
    .last_receive= 0

  },
  /* 2 */
  {
    .h= {.Instance=
#ifdef USART2
    		USART2
#else
			NULL
#endif
    },
    .tx_pin= "PA2", /* PA2,PD5 */
    .rx_pin= "PA3", /* PA3,PD6 */
    .cts_pin= NULL,
    .rts_pin= NULL,
    .gpio_af
#ifdef USART2
	= GPIO_AF_USART2
#endif
	,
    .IRQn
#ifdef USART2
	= USART2_IRQn
#endif
	,
    .tx_buffer= {
      .buf= NULL
    },
    .rx_buffer= {
      .buf= NULL
    },
    .tty= NULL,
    .last_receive= 0

  },
  /* 3 */
  {
    .h= {.Instance=
#ifdef USART3
    		USART3
#else
			NULL
#endif
			},
    .tx_pin= "PD8", /* PB10,PD8 */
    .rx_pin= "PD9", /* PB11,PD9,PC11 */
    .cts_pin= NULL,
    .rts_pin= NULL,
    .gpio_af=
#ifdef USART3
	GPIO_AF_USART3
#else
	0
#endif
	,
    .IRQn=
#ifdef USART3
	 USART3_IRQn
#else
	 0
#endif
	,
    .tx_buffer= {
      .buf= NULL
    },
    .rx_buffer= {
      .buf= NULL
    },
    .tty= NULL,
    .last_receive= 0

  },
  /* 4 */
  {
    .h= {.Instance=
#ifdef UART4
    		UART4
#else
			NULL
#endif
			},
    .tx_pin= NULL, /* PA0,PC10 */
    .rx_pin= NULL, /* PA1,PC11 */
    .cts_pin= NULL,
    .rts_pin= NULL,
    .gpio_af=
#ifdef UART4
    		GPIO_AF_UART4
#else
			0
#endif
			,
    .IRQn=
#ifdef UART4
    		UART4_IRQn
#else
			0
#endif
			,
    .tx_buffer= {
      .buf= NULL
    },
    .rx_buffer= {
      .buf= NULL
    },
    .tty= NULL,
    .last_receive= 0

  },
  /* 5 */
  {
    .h= {.Instance=
#ifdef UART5
    		UART5
#else
			NULL
#endif
			},
    .tx_pin= NULL, /* PC12 */
    .rx_pin= NULL, /* PD2 */
    .cts_pin= NULL,
    .rts_pin= NULL,
    .gpio_af=
#ifdef UART5
    		GPIO_AF_UART5
#else
			0
#endif
			,
    .IRQn=
#ifdef UART5
    		UART5_IRQn
#else
			0
#endif
			,
    .tx_buffer= {
      .buf= NULL
    },
    .rx_buffer= {
      .buf= NULL
    },
    .tty= NULL,
    .last_receive= 0

  },
  /* 6 */
  {
    .h= {.Instance=
#ifdef USART6
    		USART6
#else
			NULL
#endif
			},
    .tx_pin= "PC6", /* PC6,PG14 */
    .rx_pin= "PC7", /* PC7,PG9 */
    .cts_pin= NULL,
    .rts_pin= NULL,
    .gpio_af=
#ifdef USART6
    		GPIO_AF_USART6
#else
			0
#endif
			,
    .IRQn=
#ifdef USART6
    		USART6_IRQn
#else
			0
#endif
			,
    .tx_buffer= {
      .buf= NULL
    },
    .rx_buffer= {
      .buf= NULL
    },
    .tty= NULL,
    .last_receive= 0

  },
  /* 7 */
  {
    .h= {.Instance=
#ifdef UART7
    		UART7
#else
			NULL
#endif
			},
    .tx_pin= NULL,
    .rx_pin= NULL,
    .cts_pin= NULL,
    .rts_pin= NULL,
    .gpio_af=
#ifdef UART7
    		GPIO_AF_UART7
#else
			0
#endif
			,
    .IRQn=
#ifdef UART7
    		UART7_IRQn
#else
			0
#endif
			,
    .tx_buffer= {
      .buf= NULL
    },
    .rx_buffer= {
      .buf= NULL
    },
    .tty= NULL,
    .last_receive= 0

  },
  /* 8 */
  {
    .h= {.Instance=
#ifdef UART8
    		UART8
#else
			NULL
#endif
			},
    .tx_pin= NULL,
    .rx_pin= NULL,
    .cts_pin= NULL,
    .rts_pin= NULL,
    .gpio_af=
#ifdef UART8
    		GPIO_AF_UART8
#else
			0
#endif
			,
    .IRQn=
#ifdef UART8
    		UART8_IRQn
#else
			0
#endif
			,
    .tx_buffer= {
      .buf= NULL
    },
    .rx_buffer= {
      .buf= NULL
    },
    .tty= NULL,
    .last_receive= 0

  },
};


static volatile int cnt_irq, cnt_tx, cnt_rx, cnt_last, cnt_cont;
static volatile int cnt_rec, cnt_ovf;


/************************************************************************ OUTPUT */

/* General sending start */

static void dev_ttest_send(struct dev_usart_device_t *dev);
static char dev_ttest_queueit(struct dev_usart_device_t *dev, char c, int force);

static void
dev_start_send(struct dev_usart_device_t *dev, struct usart_tx_buffer *tx)
{
  if (tx->sending)
    {
      /* Sending is in progress already, do nothing */
      //printf("usart start: sending\n");
    }
  else
    {
      if (tx->first_occupied != tx->first_free)
	{
	  char c= tx->buf[tx->first_occupied];
	  tx->first_occupied++;
	  tx->first_occupied%= USART_TX_BUFFER_SIZE;
	  tx->sending= tx->sent_ever= 1;
	  dev->h.Instance->TDR= c;
	  EN_TC;
	}
    }
}

/* General send procedure */

static char
dev_usart_queueit(struct dev_usart_device_t *dev, char c, int force)
{
  int new, fo;
  struct usart_tx_buffer *tx;

  tx= &(dev->tx_buffer);

  DI_TC;
  new= tx->first_free+1;
  new= new % USART_TX_BUFFER_SIZE;
  if (new == tx->first_occupied)
    {
      /* need wait for room */
      //printf("usart queueit: wait room\n");
      if (force)
	{
	  /* Drop oldest char */
	  fo= tx->first_occupied+1;
	  fo= fo % USART_TX_BUFFER_SIZE;
	  tx->first_occupied= fo;
	}
      else
	{
	  /* Wait at least 1 char to be sent */
	  if (tx->sending)
	    {
	      EN_TC;
	    }
	  else
	    dev_start_send(dev, tx);
	  do {
	    DI_TC;
	    fo= tx->first_occupied;
	    EN_TC;
	    if (new == fo)
	      {
		volatile int j;
		for (j= 0; j<5000; j++)
		  ;
	      }
	  }
	  while (new == fo);
	  DI_TC;
	}
    }
  //printf("usart queueit: %c\n", c);
  tx->buf[tx->first_free]= c;
  tx->first_free= new;
  if (tx->sending)
    {
      EN_TC;
    }

  return c;
}

int
dev_usart_write(int device, int fid, void *buf, int nr)
{
  struct dev_usart_device_t *dev;
  USART_TypeDef *usart;
  int i;

  if (device>=USART_NUOF_DEVICES)
    return -1;
  dev= &dev_usart_devices[device];
  usart= dev->h.Instance;
  if (usart==NULL)
    return -2;

  if (nr > 0)
    {
      cnt_irq= cnt_tx= cnt_rx= cnt_last= cnt_cont= 0;
      for (i= 0; i<nr; i++)
	{
	  char c= ((char*)buf)[i];
	  dev_usart_queueit(dev, c, 0);
	}
      dev_start_send(dev, &(dev->tx_buffer));
      return nr;
    }

  return -1;
}


/*************************************************************************** ISR */
volatile int s1, s2, s3;

static void
dev_usart_interrupt(struct dev_usart_device_t *dev)
{
  struct usart_tx_buffer *tx;
  struct usart_rx_buffer *rx;
  struct tty_t *tty= dev->tty;

  cnt_irq++;

  s1= __HAL_UART_GET_FLAG(&(dev->h),UART_FLAG_RXNE);
  if (s1)
    {
      int new;
      char c;
      s2= __HAL_UART_GET_FLAG(&(dev->h),UART_FLAG_RXNE);
      c= dev->h.Instance->RDR & 0xff;
      __HAL_UART_CLEAR_FLAG(&(dev->h), UART_FLAG_RXNE);
      s3= __HAL_UART_GET_FLAG(&(dev->h),UART_FLAG_RXNE);
      cnt_rx++;
      if (!tty)
	{
	  rx= &(dev->rx_buffer);
	  new= rx->first_free+1;
	  new= new % USART_RX_BUFFER_SIZE;
	  if (new == rx->first_occupied)
	    {
	      cnt_ovf++;
	    }
	  else if (rx->buf != NULL)
	    {
	      rx->buf[rx->first_free]= c;
	      rx->first_free= new;
	      cnt_rec++;
	      if (dev->termio.c_lflag & ECHO)
		{
		  dev_usart_queueit(dev, c, 1);
		  dev_start_send(dev, &(dev->tx_buffer));
		}
	    }
	}
      else
	{
	  struct tty_buf *ib= &(tty->ib);
	  new= ib->first_free+1;
	  new= new % tty_ibuf_size;
	  if (new == /*rx*/ib->first_occupied)
	    {
	      cnt_ovf++;
	    }
	  else if (1)
	    {
		  tty_received(tty, c, 0);
	      cnt_rec++;
	      dev_ttest_send(dev);
	    }
	}
    }
  
  if (__HAL_UART_GET_IT_SOURCE(&(dev->h), UART_IT_TC))
    {
      __HAL_UART_CLEAR_FLAG
	  (&(dev->h), UART_FLAG_TC);
      if (!tty)
	{
	  tx= &(dev->tx_buffer);
	  cnt_tx++;
	  if (tx->first_free == tx->first_occupied)
	    {
	      DI_TC;
	      tx->sending= 0;
	      cnt_last++;
	    }
	  else
	    {
	      dev->h.Instance->TDR= ( tx->buf[tx->first_occupied++]);
	      tx->first_occupied%= USART_TX_BUFFER_SIZE;
	      cnt_cont++;
	    }
	}
      else
	{
	  struct tty_buf *ob= &(tty->ob);
	  cnt_tx++;
	  if (ob->first_free == ob->first_occupied)
	    {
	      DI_TC;
	      dev->dsending= 0;
	      cnt_last++;
	    }
	  else
	    {
	      	  int c= tty_oget(tty);
	      	  if (c >= 0)
	    	  dev->h.Instance->TDR=(c);
	      	  cnt_cont++;
	    }
	}
    }
}

void
USART1_IRQHandler(void)
{
  dev_usart_interrupt(&dev_usart_devices[1]);
  NVIC_ClearPendingIRQ(USART1_IRQn);
}

void
USART2_IRQHandler(void)
{
  dev_usart_interrupt(&dev_usart_devices[2]);
  NVIC_ClearPendingIRQ(USART2_IRQn);
}

/*
void
USART3_IRQHandler(void)
{
  usart_interrupt(&usart_devices[3]);
  NVIC_ClearPendingIRQ(USART3_IRQn);
}
*/

/*
void
UART4_IRQHandler(void)
{
  usart_interrupt(&usart_devices[4]);
  NVIC_ClearPendingIRQ(USART4_IRQn);
}
*/

/*
void
UART5_IRQHandler(void)
{
  usart_interrupt(&usart_devices[5]);
  NVIC_ClearPendingIRQ(USART5_IRQn);
}
*/

int jaj= 0;
#ifndef BRD_L433
#ifndef BRD_N491
void
USART6_IRQHandler(void)
{
  dev_usart_interrupt(&dev_usart_devices[6]);
  NVIC_ClearPendingIRQ(USART6_IRQn);
}
#endif
#endif

void
dev_usart_clk_on(USART_TypeDef *usart)
{
  if (usart == dev_usart_devices[1].h.Instance)
    __HAL_RCC_USART1_CLK_ENABLE();
  else if (usart == dev_usart_devices[2].h.Instance)
    __HAL_RCC_USART2_CLK_ENABLE();
  //else if (usart == usart_devices[3].h.Instance)
  //  __HAL_RCC_USART3_CLK_ENABLE();
  //else if (usart == usart_devices[4].h.Instance)
  //  __HAL_RCC_UART4_CLK_ENABLE();
  //else if (usart == usart_devices[5].h.Instance)
  //  __HAL_RCC_UART5_CLK_ENABLE();
#ifndef BRD_L433
#ifndef BRD_N491
  else if (usart == dev_usart_devices[6].h.Instance)
    __HAL_RCC_USART6_CLK_ENABLE();
#endif
#endif
  /*
  else if (usart == usart_devices[7].h.Instance)
    __HAL_RCC_UART7_CLK_ENABLE();
  */
  /*
  else if (usart == usart_devices[8].h.Instance)
    __HAL_RCC_UART8_CLK_ENABLE();
  */
}

int
dev_usart_init(struct dev_usart_device_t *dev)
{
  USART_TypeDef *usart;
  struct termios *t;
  
  usart= dev->h.Instance;
  if (usart==NULL)
    return -2;
  
  __HAL_UART_DISABLE(&(dev->h));
  
  dev_usart_clk_on(usart);
  
  /* Pins */
  if (dev->tx_pin!=NULL)
    {
      dev_gpio_soutput(dev->tx_pin, 1);
      dev_gpio_af(dev->tx_pin, dev->gpio_af);
    }
  if (dev->rx_pin!=NULL)
    {
      dev_gpio_input(dev->rx_pin);
      dev_gpio_af(dev->rx_pin, dev->gpio_af);
    }

  /* Setting up usart */
  if (dev->tty)
    t= &(dev->tty->termios);
  else
    t= &(dev->termio);
  // speed
  dev->h.Init.BaudRate= cfgetospeed(t);
  // word
  switch (t->c_cflag & CSIZE)
    {
    case CS5: dev->h.Init.WordLength= UART_WORDLENGTH_8B;break;
    case CS6: dev->h.Init.WordLength= UART_WORDLENGTH_8B;break;
    case CS7: dev->h.Init.WordLength= UART_WORDLENGTH_8B;break;
    case CS8: dev->h.Init.WordLength= UART_WORDLENGTH_8B;break;
    }
  // stops
  if (t->c_cflag & CSTOPB)
    dev->h.Init.StopBits= UART_STOPBITS_2;
  else
    dev->h.Init.StopBits= UART_STOPBITS_1;
  // parity
  if (t->c_cflag & PARENB)
    {
      if (t->c_cflag & PARODD)
	dev->h.Init.Parity= UART_PARITY_ODD;
      else
	dev->h.Init.Parity= UART_PARITY_EVEN;
    }
  else
    dev->h.Init.Parity= UART_PARITY_NONE;
  // others
  dev->h.Init.Mode= UART_MODE_TX_RX;
  dev->h.Init.OverSampling= UART_OVERSAMPLING_16;
  HAL_UART_Init(&(dev->h));

  /* Interrupt */
  HAL_NVIC_EnableIRQ(dev->IRQn);
  
  /* Setup buffers */
  if (dev->tx_buffer.buf == NULL)
    dev->tx_buffer.buf= (char*)malloc(USART_TX_BUFFER_SIZE);
  dev->tx_buffer.first_occupied= 0;
  dev->tx_buffer.first_free= 0;
  dev->tx_buffer.sending= 0;
  dev->tx_buffer.sent_ever= 0;

  if (dev->rx_buffer.buf == NULL)
    dev->rx_buffer.buf= (char*)malloc(USART_RX_BUFFER_SIZE);
  dev->rx_buffer.first_occupied= 0;
  dev->rx_buffer.first_free= 0;

  cnt_rec= 0;
  cnt_ovf= 0;
  EN_RX;
  __HAL_UART_ENABLE(&(dev->h));

  return 0;
}


int
dev_usart_open(int device, int fid, int fo_flags)
{
  struct dev_usart_device_t *dev;
  USART_TypeDef *usart;

  if (device>=USART_NUOF_DEVICES)
    {
      //printf("open usart%d error: no such device\n");
      return -1;
    }
  dev= &dev_usart_devices[device];
  usart= dev->h.Instance;
  if (usart==NULL)
    {
      //printf("open usart%d error: no such usart\n");
      return -2;
    }

  return dev_usart_init(dev);
}

int
dev_usart_close(int device, int fid)
{
  struct dev_usart_device_t *dev;
  USART_TypeDef *usart;
  
  if (device>=USART_NUOF_DEVICES)
    return -1;
  dev= &dev_usart_devices[device];
  usart= dev->h.Instance;
  if (usart==NULL)
    return -2;
  
  DI_RX;
  DI_TC;
  //f4_gpio_set("PD12", 0);
  __HAL_UART_DISABLE(&(dev->h));

  /* Interrupt */
  HAL_NVIC_DisableIRQ(dev->IRQn);
 
  return 0;
}


/************************************************************************* INPUT */

static double
mynow(void)
{
  struct timeval tv;

  gettimeofday(&tv, NULL);
  return (double)tv.tv_sec + ((double)tv.tv_usec/1000000.0);
}

int
dev_usart_read(int device, int fid, void *buf, int nr)
{
  int ff, fo, new, i;
  USART_TypeDef *usart;
  struct usart_rx_buffer *rx;
  double started, now, elapsed;
  struct dev_usart_device_t *dev;
  
  if (device>=USART_NUOF_DEVICES)
    return -1;
  dev= &dev_usart_devices[device];
  usart= dev->h.Instance;
  if (usart==NULL)
    return -2;
  
  rx= &(dev_usart_devices[device].rx_buffer);
  started= mynow();

  for (i= 0; i<nr; i++)
    {
      fo= rx->first_occupied;
      do {
	DI_RX;
	ff= rx->first_free;
	EN_RX;
	if (ff==fo)
	  {
	    volatile int j;
	    return i;
	    for (j=0;j<5000;j++) ;
	  }
	now= mynow();
	elapsed= now - started;
	if (elapsed > 2.0)
	  {
	    //printf("usart%d: timeout %f - %f=%f\n", device, now, started, elapsed);
	    return i;
	  }
      }
      while (ff==fo);
      
      new= fo+1;
      new= new % USART_RX_BUFFER_SIZE;
      ((char*)buf)[i]= rx->buf[fo];
      DI_RX;
      EN_RX;
    }

  return nr;
}

int
dev_usart_input_avail(int device, int fid)
{
  int ff, fo;
  struct dev_usart_device_t *dev;
  USART_TypeDef *usart;
  struct usart_rx_buffer *rx;
  
  if (device>=USART_NUOF_DEVICES)
    return 0;
  dev= &dev_usart_devices[device];
  usart= dev->h.Instance;
  if (usart==NULL)
    return 0;
  
  rx= &(dev_usart_devices[device].rx_buffer);
  DI_RX;
  ff= rx->first_free;
  fo= rx->first_occupied;
  EN_RX;
  if (ff==fo)
    return 0;
  return (ff<fo)?(fo-ff):(ff-fo);
}

int
dev_usart_ioctl(int device, int fid, unsigned long int request, va_list ap)
{
  USART_TypeDef *usart;
  struct dev_usart_device_t *dev;
  struct termios *tp;
  double *d;
  int i;
  tcflag_t cflag;

  if (device>=USART_NUOF_DEVICES)
    return -1;
  dev= &dev_usart_devices[device];
  usart= dev->h.Instance;
  if (usart==NULL)
    return 0;

  switch (request)
    {
    case DIOGTTY:
      tp= va_arg(ap, struct termios *);
      if (dev->tty)
	memcpy(tp, &(dev->tty->termios), sizeof(struct termios));
      else
	memcpy(tp, &(dev->termio), sizeof(struct termios));
      return 0;
      break;
    case DIOSTTY:
      tp= va_arg(ap, struct termios *);
      cflag= dev->termio.c_cflag;
      dev->termio.c_iflag= tp->c_iflag;
      dev->termio.c_oflag= tp->c_oflag;
      dev->termio.c_cflag= tp->c_cflag;
      dev->termio.c_lflag= tp->c_lflag;
      if (dev->tty)
	{
	  cflag= dev->tty->termios.c_cflag;
	  dev->tty->termios.c_iflag= tp->c_iflag;
	  dev->tty->termios.c_oflag= tp->c_oflag;
	  dev->tty->termios.c_cflag= tp->c_cflag;
	  dev->tty->termios.c_lflag= tp->c_lflag;
	}
      for (i= 0; i<NCCS; i++)
	{
	  dev->termio.c_cc[i]= tp->c_cc[i];
	  if (dev->tty)
	    dev->tty->termios.c_cc[i]= tp->c_cc[i];
	}
      if ((dev->termio.c_speed != tp->c_speed) ||
	  (cflag != tp->c_cflag) ||
	  (dev->tty &&
	   (dev->tty->termios.c_speed != tp->c_speed)))
	{
	  cfsetspeed(&(dev->termio), tp->c_speed);
	  if (dev->tty)
	    cfsetspeed(&(dev->tty->termios), tp->c_speed);
	  dev_usart_init(dev);
	}
      return 0;
      break;
    case DIOGLASTRECTIME:
      d= va_arg(ap, double *);
      memcpy(d, &(dev->last_receive), sizeof(double));
      return 0;
      break;
    }

  return 0;
}  


int
dev_ttest_open(int device, int fid, int fo_flags)
{
  struct dev_usart_device_t *dev;

  if (device>=USART_NUOF_DEVICES)
    return -1;
  dev= &dev_usart_devices[device];
  dev->tty= malloc(sizeof(struct tty_t));
  tty_init(dev->tty);
  
  return dev_usart_open(device, fid, fo_flags);
}

int
dev_ttest_close(int device, int fid)
{
  struct dev_usart_device_t *dev;
  int ret;
  
  dev= &dev_usart_devices[device];
  ret= dev_usart_close(device, fid);
  free(dev->tty);
  return ret;
}

int
dev_ttest_read(int device, int fid, void *buf, int nr)
{
  int ff, fo, new, i;
  struct dev_usart_device_t *dev;
  USART_TypeDef *usart;
  struct usart_rx_buffer *rx;
  double started, now, elapsed;
  
  if (device>=USART_NUOF_DEVICES)
    return -1;
  dev= &dev_usart_devices[device];
  usart= dev->h.Instance;
  if (usart==NULL)
    return -2;
  
  DI_RX;
  i= tty_read(dev_usart_devices[device].tty, buf, nr);
  EN_RX;
  return i;
  
  rx= &(dev_usart_devices[device].rx_buffer);
  started= mynow();

  for (i= 0; i<nr; i++)
    {
      fo= rx->first_occupied;
      do {
	DI_RX;
	ff= rx->first_free;
	EN_RX;
	if (ff==fo)
	  {
	    volatile int j;
	    return i;
	    for (j=0;j<5000;j++) ;
	  }
	now= mynow();
	elapsed= now - started;
	if (elapsed > 2.0)
	  {
	    //printf("usart%d: timeout %f - %f=%f\n", device, now, started, elapsed);
	    return i;
	  }
      }
      while (ff==fo);
      
      new= fo+1;
      new= new % USART_RX_BUFFER_SIZE;
      ((char*)buf)[i]= rx->buf[fo];
      DI_RX;
      rx->first_occupied= new;
      EN_RX;
    }

  return nr;
}

int
dev_ttest_input_avail(int device, int fid)
{
  int ff, fo;
  struct dev_usart_device_t *dev;
  USART_TypeDef *usart;
  struct usart_rx_buffer *rx;
  
  if (device>=USART_NUOF_DEVICES)
    return 0;
  dev= &dev_usart_devices[device];
  usart= dev->h.Instance;
  if (usart==NULL)
    return 0;
  
  DI_RX;
  ff= tty_readable(dev_usart_devices[device].tty);
  EN_RX;
  return ff;
  
  rx= &(dev_usart_devices[device].rx_buffer);
  DI_RX;
  ff= rx->first_free;
  fo= rx->first_occupied;
  EN_RX;
  if (ff==fo)
    return 0;
  return (ff<fo)?(fo-ff):(ff-fo);
  return -1;
}

static void
dev_ttest_send(struct dev_usart_device_t *dev)
{
  USART_TypeDef *USART= dev->h.Instance;
  struct tty_t *tty= dev->tty;
  struct tty_buf *ob= &(tty->ob);

  if (dev->dsending)
    {
      /* Sending is in progress already, do nothing */
      //printf("usart start: sending\n");
    }
  else
    {
      if (ob->first_occupied != ob->first_free)
	{
	  char c= tty->obuf[ob->first_occupied];
	  //printf("usart start[%d]: %c\n", tx->first_occupied, c);
	  ob->first_occupied++;
	  ob->first_occupied%= tty_obuf_size;
	  dev->dsending= dev->dsent_ever= 1;
	  USART->TDR= c;
	  EN_TC;
	}
    }
}

static char
dev_ttest_queueit(struct dev_usart_device_t *dev, char c, int force)
{
  int new, fo;
  struct tty_buf *ob;
  
  ob= &(dev->tty->ob);
  
  DI_TC;
  new= ob->first_free+1;
  new= new % tty_obuf_size;
  if (new == ob->first_occupied)
    {
      /* need wait for room */
      //printf("usart queueit: wait room\n");
      if (force)
	{
	  /* Drop oldest char */
	  fo= ob->first_occupied+1;
	  fo= fo % tty_obuf_size;
	  ob->first_occupied= fo;
	}
      else
	{
	  /* Wait at least 1 char to be sent */
	  if (dev->dsending)
	    {
	      EN_TC;
	    }
	  else
	    dev_ttest_send(dev);
	  do {
	    DI_TC;
	    //dev_gpio_set("PD12", 0);
	    fo= ob->first_occupied;
	    EN_TC;
	    if (new == fo)
	      {
		volatile int j;
		for (j= 0; j<5000; j++)
		  ;
	      }
	  }
	  while (new == fo);
	  DI_TC;
	}
    }
  //printf("usart queueit: %c\n", c);
  dev->tty->obuf[/*tx*/ob->first_free]= c;
  ob->first_free= new;
  if (dev->dsending)
    {
      EN_TC;
    }

  return c;
}

int
dev_ttest_write(int device, int fid, void *buf, int nr)
{
  struct dev_usart_device_t *dev;
  USART_TypeDef *usart;
  int i;

  if (device>=USART_NUOF_DEVICES)
    return -1;
  dev= &dev_usart_devices[device];
  usart= dev->h.Instance;
  if (usart==NULL)
    return -2;

  if (nr > 0)
    {
      cnt_irq= cnt_tx= cnt_rx= cnt_last= cnt_cont= 0;
      for (i= 0; i<nr; i++)
	{
	  char c= ((char*)buf)[i];
	  //printf("usart%d: write %c\n", device, c);
	  dev_ttest_queueit(dev, c, 0);
	}
      dev_ttest_send(dev);
      return nr;
    }
  return -1;
}
