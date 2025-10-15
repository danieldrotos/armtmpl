#include <stdio.h>

#include "brd.h"
#include "utils.h"
#include "terminal.h"
#include "pin.h"

#include "ask.h"

int at_inited= 0;

void ask_test_init()
{
  //ask_led_init();
  pin_pwm(ASK_BUZZER, 50);
  tu_color(0,7);
  tu_show();
  tu_clear_screen();
  at_inited= 1;
}

void ask_test()
{
  static uint led, buz;
  static int val, fr;
  unsigned int now= HAL_GetTick();
  if (!at_inited) ask_test_init();
  if (now - led > 500)
    {
      if (val > 50)
        val= 30;
      else
        val= 60;
      pin_bright(ASK_LED, val);
      //pin_toggle(ASK_LED);
      led= now;
    }
  if (now - buz > 2000)
    {
      //pin_pwm(ASK_BUZZER, val);
      //pin_toggle(ASK_BUZZER);
      if ((fr <= 0) || (fr > 2000))
        fr= 100;
      else
        fr+= 100;
      pin_freq(ASK_BUZZER, fr);
      buz= now;
    }
  if (pin_pressed(ASK_BUTTON))
    {
      printf("Button pressed \n");
    }
  if (pin_released(ASK_BUTTON))
    {
      printf("Button released\n");
    }
  {
    int v= pin_get(ASK_POTM);
    tu_go(1,1);
    printf("POTM=%5d", v);
    v= pin_get(ASK_LIGHT);
    tu_go(20,1);
    printf("LIGHT=%5d", v);
    v= pin_get(ASK_SOUND);
    tu_go(40,1);
    printf("SOUND=%5d", v);
    printf("\n");
  }
}


/*
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  //pin_toggle(ASK_LED);
}
*/

ALIGN(4) int exti_counter;
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == GPIO_PIN_7)
    exti_counter++;
}

