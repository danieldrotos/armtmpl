#include "brd.h"
#ifdef BRD_L433

#include "brd_l433.h"


struct pin_t brd_pins[]= {
    PIN_OPIN("PB13"),
    PIN_BTNL("PC13"),
    PIN_NONE
};


void
brd_type_init()
{
}


#endif

/* End of brd_l433.c */
