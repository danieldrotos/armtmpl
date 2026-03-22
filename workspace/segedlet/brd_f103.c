/*
 * brd_f103.c
 *
 *  Created on: Mar 18, 2026
 *      Author: drdani
 */

#include "brd.h"
#ifdef BRD_F103

#include "brd_f103.h"


struct pin_t brd_pins[]= {
    PIN_OPIN("PC13"),
    PIN_NONE
};


void
brd_type_init()
{
  //HAL_TIM_Base_Start_IT(&htim5);
}
#endif
