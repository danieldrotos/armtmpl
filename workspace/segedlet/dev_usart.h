#ifndef DEV_USART_HEADER
#define DEV_USART_HEADER

#include <stdarg.h>

#include "wrap_termios.h"

#include "ols_tty.h"

#include "brd.h"


#define USART_NUOF_DEVICES 9


#define USART_TX_BUFFER_SIZE 128
#define USART_RX_BUFFER_SIZE 128

struct usart_tx_buffer {
  char *buf;
  volatile int first_occupied;
  volatile int first_free;
  volatile char sending;
  volatile char sent_ever;
};

struct usart_rx_buffer {
  char *buf;
  volatile int first_occupied;
  volatile int first_free;
};

struct dev_usart_device_t {
  //USART_TypeDef *usart;
  UART_HandleTypeDef h;
  char *tx_pin;
  char *rx_pin;
  char *rts_pin;
  char *cts_pin;
  //uint32_t rcc_apb1; /* non-0 value needs apb1 enabled */
  //uint32_t rcc_apb2; /* non-0 value needs apb2 enabled */
  uint8_t  gpio_af;
  /*enum IRQn*/IRQn_Type IRQn;
  struct usart_tx_buffer tx_buffer;
  struct usart_rx_buffer rx_buffer;
  struct termios termio;
  struct tty_t *tty;
  volatile char dsending, dsent_ever;
  double last_receive;
};

extern struct dev_usart_device_t dev_usart_devices[USART_NUOF_DEVICES];

extern int dev_usart_write(int device, int fid, void *buf, int nr);
extern int dev_usart_init(struct dev_usart_device_t *dev);
extern int dev_usart_open(int device, int fid, int fo_flags);
extern int dev_usart_close(int device, int fid);
extern int dev_usart_read(int device, int fid, void *buf, int nr);
extern int dev_usart_input_avail(int device, int fid);
extern int f4_usart_tx_free_space(int device, int fid);

extern int dev_usart_ioctl(int device, int fid, unsigned long int request, va_list ap);

extern int dev_ttest_open(int device, int fid, int fo_flags);
extern int dev_ttest_close(int device, int fid);
extern int dev_ttest_read(int device, int fid, void *buf, int nr);
extern int dev_ttest_input_avail(int device, int fid);
extern int dev_ttest_write(int device, int fid, void *buf, int nr);

// Map old AF names to HAL versions
#define GPIO_AF_USART1 GPIO_AF7_USART1
#define GPIO_AF_USART2 GPIO_AF7_USART2
#define GPIO_AF_USART3 GPIO_AF7_USART3
#define GPIO_AF_UART4  GPIO_AF8_UART4
#define GPIO_AF_UART5  GPIO_AF8_UART5
#define GPIO_AF_USART6 GPIO_AF8_USART6
//#define GPIO_AF_UART7  GPIO_AF8_UART7
//#define GPIO_AF_UART8  GPIO_AF8_UART8

//extern int ols_drv_usart_install(void);


#endif
