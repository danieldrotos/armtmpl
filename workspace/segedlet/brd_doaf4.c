#include "brd.h"
#ifdef BRD_DOAF4

#include "brd_doaf4.h"


struct pin_t brd_pins[]= {
    PIN_OPIN("PC8"),
    PIN_OPIN("PC9"),
    PIN_OPIN("PC10"),
    PIN_OPIN("PC11"),
    PIN_BTNL("PC12"),
    PIN_BTNL("PC13"),
    PIN_BTNL("PC14"),
    PIN_BTNL("PC15"),
    PIN_NONE
};


#include "dev_usart.h"

void
brd_type_init()
{
  dev_usart_devices[2].tx_pin= "PA2";
  dev_usart_devices[2].rx_pin= "PA3";
}


#endif

/* End of brd_doaf4.c */
