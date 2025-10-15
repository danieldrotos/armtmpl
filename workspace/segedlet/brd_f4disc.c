#include "brd.h"
#ifdef BRD_F4DISC

#include "brd_f4disc.h"


struct pin_t brd_pins[]= {
    PIN_OPIN("PD12"),
    PIN_OPIN("PD13"),
    PIN_OPIN("PD14"),
    PIN_OPIN("PD15"),
    PIN_BTNL("PA0"),
    PIN_NONE
};


void
brd_type_init()
{

}


#endif

/* End of brd_f4disc.c */
