#ifndef BRD_DOAF4_HEADER
#define BRD_DOAF4_HEADER


enum {
  LED0= 0, // PC8
  LED1= 1, // PC9
  LED2= 2, // PC10
  LED3= 3, // PC11
  BTN0= 4, // PC12
  BTN1= 5, // PC13
  BTN2= 6, // PC14
  BTN3= 7  // PC15
};

#define TDR      DR
#define RDR      DR

#define BRD_STDIO "/dev/usart1"
#define BRD_USART 1
#define BRD_TX_PIN "PA9"
#define BRD_RX_PIN "PA10"


#endif

/* End of brd_doaf4.h */
