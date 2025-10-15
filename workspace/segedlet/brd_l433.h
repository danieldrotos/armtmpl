#ifndef BRD_L433_HEADER
#define BRD_L433_HEADER


// LED is on PA5 or PB13 (Arduino D13)
enum {
  LED0= 0, //1<<13, // green PB13 / TIM1.CH1N TIM15.CH1N
  BTN0= 1  // BTN0 PC13
};


#define TDR      TDR
#define RDR      RDR

#define BRD_STDIO "/dev/usart2"
#define BRD_USART 2
#define BRD_TX_PIN "PA2"
#define BRD_RX_PIN "PA3"


#endif

/* End of brd_l433.h */
