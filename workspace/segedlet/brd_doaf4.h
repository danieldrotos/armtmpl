#ifndef BRD_DOAF4_HEADER
#define BRD_DOAF4_HEADER


enum {
  LED0= 1, // PC8
  LED1= 2, // PC9
  LED2= 3, // PC10
  LED3= 4, // PC11
  BTN0= 5, // PC12
  BTN1= 6, // PC13
  BTN2= 7, // PC14
  BTN3= 8  // PC15
};

#define TDR      DR
#define RDR      DR

#define BRD_STDIO "/dev/usart1"
#define BRD_USART 1
#define BRD_TX_PIN "PA9"
#define BRD_RX_PIN "PA10"


#endif

/* End of brd_doaf4.h */
