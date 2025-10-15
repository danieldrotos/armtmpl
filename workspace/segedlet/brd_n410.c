#include "brd.h"
#ifdef BRD_N410

#include "brd_n410.h"


struct pin_t brd_pins[]= {
    PIN_OPIN("PA5"),
    PIN_BTNL("PC13"),
    PIN_NONE
};


void
brd_type_init()
{
}


#endif

/* End of brd_n410.c */
