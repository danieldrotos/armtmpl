#include "dev_timer.h"


void dev_timer_pwm_start(TIM_HandleTypeDef *h, int ch)
{
  switch (ch)
  {
    case 1: HAL_TIM_PWM_Start(h, TIM_CHANNEL_1); break;
    case 2: HAL_TIM_PWM_Start(h, TIM_CHANNEL_2); break;
    case 3: HAL_TIM_PWM_Start(h, TIM_CHANNEL_3); break;
    case 4: HAL_TIM_PWM_Start(h, TIM_CHANNEL_4); break;
  }
}

void dev_timer_set_duty(TIM_HandleTypeDef *h, int ch, float duty)
{
  uint32_t arr, cl;
  arr= h->Instance->ARR;
  if (duty <   0) duty= 0;
  if (duty > 100) duty= 100;
  cl= arr*(duty/100.0);
  switch (ch)
  {
    case 1: h->Instance->CCR1= cl; break;
    case 2: h->Instance->CCR2= cl; break;
    case 3: h->Instance->CCR3= cl; break;
    case 4: h->Instance->CCR4= cl; break;
  }
}

void dev_timer_set_freq(TIM_HandleTypeDef *h, int ch, int fr)
{
  uint32_t arr, cl;
  float d;
  if (fr < 1)
    fr= 1;
  if (fr > 1000000)
    fr= 1000000;
  arr= h->Instance->ARR;
  switch (ch)
  {
    case 1: cl= h->Instance->CCR1; break;
    case 2: cl= h->Instance->CCR2; break;
    case 3: cl= h->Instance->CCR3; break;
    case 4: cl= h->Instance->CCR4; break;
  }
  d= (float)cl*100.0/(float)arr;
  h->Instance->ARR= 1000000/fr;
  dev_timer_set_duty(h, ch, d);
}

