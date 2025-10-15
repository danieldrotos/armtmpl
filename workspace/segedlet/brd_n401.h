#ifndef BRD_N401_HEADER
#define BRD_N401_HEADER


enum {
  LED0= 0,
  BTN0= 1
};


#define TDR      DR
#define RDR      DR

#define BRD_STDIO "/dev/usart2"
#define BRD_USART 2
#define BRD_TX_PIN "PA2"
#define BRD_RX_PIN "PA3"


#endif

/* End of brd_n401.h */
