#include "brd.h"
#ifdef BRD_N411

#include "brd_n411.h"


struct pin_t brd_pins[]= {
    PIN_PPIN("PA5", htim2, 1, GPIO_AF1_TIM2),
    PIN_BTNL("PC13"),
    PIN_NONE
};

void
brd_type_init()
{
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
  HAL_TIM_Base_Start_IT(&htim5);
}


#endif

/* End of brd_n411.c */
