#ifndef ASK_H_
#define ASK_H_

#include <stdint.h>

#include "brd.h"
#include "gpio.h"

#define ASK_GPIO_LED		"PB10" // TIM2.CH3 (401,411)
#define ASK_GPIO_BUZZER		"PB4"  // TIM3.CH1 (401,411)
#define ASK_GPIO_BUTTON         "PB5"  //
#define ASK_GPIO_POTMETER       "PA0"  // ADC1.0 (401,410,411)
#define ASK_GPIO_LIGHT          "PB0"  // ADC1.8 (401,410,411)
#define ASK_GPIO_SOUND          "PA4"  // ADC1.4 (401,410,411)


extern struct pin_t ask_led;
extern struct pin_t ask_buzzer;
extern struct pin_t ask_button;
extern struct pin_t ask_potm;
extern struct pin_t ask_light;
extern struct pin_t ask_sound;

#define ASK_LED    (&ask_led)
#define ASK_BUZZER (&ask_buzzer)
#define ASK_BUTTON (&ask_button)
#define ASK_POTM   (&ask_potm)
#define ASK_LIGHT  (&ask_light)
#define ASK_SOUND  (&ask_sound)


#endif /* ASK_H_ */
