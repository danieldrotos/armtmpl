#ifndef DEV_TIMER_H_
#define DEV_TIMER_H_

#include "dev.h"


extern void dev_timer_pwm_start(TIM_HandleTypeDef *h, int ch);
extern void dev_timer_set_duty(TIM_HandleTypeDef *h, int ch, float duty);
extern void dev_timer_set_freq(TIM_HandleTypeDef *h, int ch, int fr);


#endif /* DEV_TIMER_H_ */
