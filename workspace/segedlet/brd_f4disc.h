#ifndef BRD_F4DISC_HEADER
#define BRD_F4DISC_HEADER


enum {
  LED0= 1, // green PD12 / TIM4.CH1
  LED1= 2, // orange PD13 / TIM4.CH2
  LED2= 3, // red PD14 / TIM4.CH3
  LED3= 4, // blue PD15 / TIM4.CH4

  LED_GREEN= LED0,
  LED_ORANGE= LED1,
  LED_RED= LED2,
  LED_BLUE= LED3,

  BTN0= 5 // BTN0 PA0
};


#define TDR      DR
#define RDR      DR

#define BRD_STDIO "/dev/usart2"
#define BRD_USART 2
#define BRD_TX_PIN "PA2"
#define BRD_RX_PIN "PA3"


#endif

/* End of brd_f4disc.h */
