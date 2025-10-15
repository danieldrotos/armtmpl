#ifndef BRD_N410_HEADER
#define BRD_N410_HEADER


enum  {
  LED0= 0, //1<<5, // green PA5
  BTN0= 1 //1<<13 // BTN0 PC13
};

#define TDR      DR
#define RDR      DR

#define BRD_STDIO "/dev/usart2"
#define BRD_USART 2
#define BRD_TX_PIN "PA2"
#define BRD_RX_PIN "PA3"


#endif

/* End of brd_n410.h */
