#include <stdio.h>

#include "brd.h"

#include "dev_gpio.h"
#include "dev_usart.h"
//#include "f4_timer.h"
#include "ols_fs.h"
#include "ols_drv_null.h"
#include "ols_drv_usart.h"
//#include "ols_drv_vcp.h"
#include "serial.h"
//#include "stm32f4xx_hal_tim.h"
#ifdef BRD_LED_TIMER
#include "tim.h"
#endif
#include "utils.h"
#include "pin.h"


unsigned int brd_buttons;
char *brd_stdio= BRD_STDIO;
enum brd_lmode_t brd_led_mode= BRD_LMODE_NONE;

void
brd_init()
{
  //char s[10];
  unsigned int /*b, m,*/ i;

#ifdef BRD_USART
#ifdef BRD_TX_PIN
  dev_usart_devices[BRD_USART].tx_pin= BRD_TX_PIN;
#endif  
#ifdef BRD_RX_PIN
  dev_usart_devices[BRD_USART].rx_pin= BRD_RX_PIN;
#endif  
#endif
  brd_type_init();
  ols_drv_null_install();
  ols_drv_usart_install();
  ols_stdio_init(brd_stdio, brd_stdio, brd_stdio);
  tc_setsparams(0, "115200,N,8,1");
  setvbuf(stdout, NULL, _IONBF, 0);
  write(0, "\n", 1);
  
  /*
  for (b= 0, m= 1; m < 0x10000; m<<= 1, b++)
    {
      if (brd_leds & m)
	{
	  snprintf(s, 9, "%s%d", f4_gpio_name(brd_led_port), b);
	  f4_gpio_output(s);
	  brd_led_off(m);
	}
      if (brd_btns & m)
	{
	  snprintf(s, 9, "%s%d", f4_gpio_name(brd_btn_port), b);
	  f4_gpio_pin_init_of(s,
#ifdef brd_btn_mode
			      brd_btn_mode
#else
			      "IP2N"
#endif
			      );
	}
    }
    */
  //brd_sample_buttons();
  //brd_led_mode= BRD_LMODE_LED;

  for (i= 0; brd_pins[i].name; i++)
    {
      if (brd_pins[i].output)
        pin_set(&brd_pins[i], 0);
      else
        pin_get(&brd_pins[i]);
    }

  printf("\n");
}

/*
void
brd_set_led_mode(int led, enum brd_lmode_t lmode)
{
#ifndef BRD_LED_TIMER
  lmode= BRD_LMODE_LED;
#endif
  if (lmode != brd_led_mode)
    {
      char s[100];
      int b= 0;
      while ((led & (1<<b)) == 0) b++;
      snprintf(s, 99, "%s%d", f4_gpio_name(brd_led_port), b);
      if (lmode == BRD_LMODE_LED)
        {
          f4_gpio_output(s);
          brd_led_mode= lmode;
        }
#ifdef BRD_LED_TIMER
      else if (led == LED0)
        {
          f4_gpio_af(s, BRD_LED_PWM_AF);
          brd_led_mode= lmode;
        }
#endif
    }
}
*/

static struct pin_t *
brd_pget(int ID)
{
  int i;
  for (i= 0; brd_pins[i].name; i++)
    if (i == ID)
      return &brd_pins[i];
  return NULL;
}

void
brd_led_on(int led)
{
  pin_on(brd_pget(led));
  /*
  //brd_led_port->BSRR= led;
  brd_led_port->ODR |= led;
  brd_set_led_mode(led, BRD_LMODE_LED);
  */
}

void
brd_led_off(int led)
{
  pin_off(brd_pget(led));
  /*
  //brd_led_port->BSRR= (led<<16);
  brd_led_port->ODR &= ~led;
  brd_set_led_mode(led, BRD_LMODE_LED);
  */
}

void
brd_led_toggle(int led)
{
  pin_toggle(brd_pget(led));
  /*
  brd_led_port->ODR ^= led;
  brd_set_led_mode(led, BRD_LMODE_LED);
  */
}

void
brd_led_set(int led, int on)
{
  pin_set(brd_pget(led), on);
  /*
  if (on)
    //brd_led_port->BSRR= led;
    brd_led_port->ODR |= led;
  else
    //brd_led_port->BSRR= (led<<16);
    brd_led_port->ODR &= ~led;
  brd_set_led_mode(led, BRD_LMODE_LED);
  */
}

int
brd_led_get(int led)
{
  return pin_oget(brd_pget(led));
  /*
  return (brd_led_port->ODR & led)?1:0;
  */
}

void
brd_led_pwm(float power)
{
  pin_bright(&brd_pins[0], power);
  /*
#ifdef BRD_LED_TIMER
  uint32_t arr, cl;
  float d;
  brd_set_led_mode(LED0, BRD_LMODE_PWM);
  arr= BRD_LED_TIMER.Instance->BRD_ARR_NAME;
  d= 0.01 * power * power;
  cl= arr*(d/100.0);
  BRD_LED_TIMER.Instance->BRD_CL_NAME= cl;
  return cl;
#else
  if (power > 50)
    {
      brd_led_set(LED0, 1);
      return 1;
    }
  else
    {
      brd_led_set(LED0, 0);
      return 0;
    }
#endif
*/
}


//static int brd_sampled= 0;

void
brd_sample_buttons()
{
  int i;
  /*
  static unsigned int last;
  unsigned int now= msnow();
  if (now - last > 20)
    {
      brd_buttons= brd_btn_port->IDR & (brd_btns);
      last= now;
      brd_sampled= 1;
    }
    */
  for (i=0; brd_pins[i].name; i++)
    if (brd_pins[i].output == 0)
      pin_debounce(&brd_pins[i]);
}

unsigned int
brd_button(int btn)
{
  return pin_val(brd_pget(btn));
  /*
  brd_sample_buttons();
  if (brd_btn_active_state)
    return (brd_buttons & btn)?1:0;
  else
    return (brd_buttons & btn)?0:1;
    */
}
/*
static unsigned int brd_last_inited;
static unsigned int brd_last_pressed;
static unsigned int brd_last_released;
*/
/*
int
brd_button_edge_started(enum brd_btn btn)
{
  brd_sample_buttons();
  int i= brd_last_inited & btn;
  int a;
  if (!brd_sampled)
    return 0;
  if (i)
    return 1;
  a= brd_buttons & btn;
  brd_last_pressed&= ~btn;
  brd_last_released&= ~btn;
  brd_last_pressed|= a;
  brd_last_released|= a;
  brd_last_inited|= btn;
  return 0;
}
*/

int
brd_button_pressed(int btn)
{
  return pin_pressed(brd_pget(btn));
  /*
  int a, l;
  brd_sample_buttons();
  if (!brd_button_edge_started(btn))
    return 0;
  a= brd_buttons & btn;
  l= brd_last_pressed & btn;
  if (a != l)
    {
      int ab= a?1:0;
      brd_last_pressed&= ~btn;
      brd_last_pressed|= a;
      return brd_btn_active_state == ab;
    }
  return 0;
  */
}

int
brd_button_released(int btn)
{
  return pin_released(brd_pget(btn));
  /*
  int a, l;
  brd_sample_buttons();
  if (!brd_button_edge_started(btn))
    return 0;
  a= brd_buttons & btn;
  l= brd_last_released & btn;
  if (a != l)
    {
      int ab= a?1:0;
      brd_last_released&= ~btn;
      brd_last_released|= a;
      return brd_btn_active_state != ab;
    }
  return 0;
  */
}


/* End of brd.c */
