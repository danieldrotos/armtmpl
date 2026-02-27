#include <stdio.h>

#include "brd.h"

#include "dev_gpio.h"
#include "dev_usart.h"
#include "ols_fs.h"
#include "ols_drv_null.h"
#include "ols_drv_usart.h"
#include "serial.h"
#ifdef BRD_LED_TIMER
#include "tim.h"
#endif
#include "utils.h"
#include "pin.h"


char *brd_stdio= BRD_STDIO;

void
brd_init()
{
  unsigned int i;

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
  
  for (i= 0; brd_pins[i].name; i++)
    {
      if (brd_pins[i].output)
        pin_set(&brd_pins[i], 0);
      else
        pin_get(&brd_pins[i]);
    }

  printf("\n");
}

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
}

void
brd_led_off(int led)
{
  pin_off(brd_pget(led));
}

void
brd_led_toggle(int led)
{
  pin_toggle(brd_pget(led));
}

void
brd_led_set(int led, int on)
{
  pin_set(brd_pget(led), on);
}

int
brd_led_get(int led)
{
  return pin_oget(brd_pget(led));
}

void
brd_led_pwm(float power)
{
  pin_bright(&brd_pins[0], power);
}

void
brd_sample_buttons()
{
  int i;
  for (i=0; brd_pins[i].name; i++)
    if (brd_pins[i].output == 0)
      pin_debounce(&brd_pins[i]);
}

unsigned int
brd_button(int btn)
{
  return pin_ison(brd_pget(btn));
}

int
brd_button_pressed(int btn)
{
  return pin_pressed(brd_pget(btn));
}

int
brd_button_released(int btn)
{
  return pin_released(brd_pget(btn));
}


/* End of brd.c */
