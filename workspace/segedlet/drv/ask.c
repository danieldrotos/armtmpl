#include "ask.h"

#include "tim.h"
#include "adc.h"

#include "pin.h"
#include "dev_gpio.h"
#include "dev_adc.h"


/*
 * OUTPUTS
 */

/*
 *  LED: D6 = PB10 TIM2.CH3
 */

#ifdef BRD_N410
DEF_OPIN(ask_led, ASK_GPIO_LED);
#else
DEF_PPIN(ask_led, ASK_GPIO_LED, htim2, 3, GPIO_AF1_TIM2);
#endif

/*
 * BUZZER: D5 = PB4 TIM3.CH1
 */

#ifdef BRD_N410
DEF_OPIN(ask_buzzer, ASK_GPIO_BUZZER);
#else
DEF_PPIN(ask_buzzer, ASK_GPIO_BUZZER, htim3, 1, GPIO_AF2_TIM3);
#endif

/*
 * INPUTS
 */

/*
 * BUTTON: D4 = PB5
 */

DEF_BTNH(ask_button, ASK_GPIO_BUTTON);


/*
 * POTENTIOMETER: A0 = PA0 ADC1.IN0
 */

DEF_AN(ask_potm, ASK_GPIO_POTMETER, hadc1, 0);


/*
 * LIGHT_SENSOR: A3 = PB0 ADC1.IN8
 */

DEF_AN(ask_light, ASK_GPIO_LIGHT, hadc1, 8);


/*
 * SOUND_SENSOR: A2 = PA4 ADC1.CH4
 */

DEF_AN(ask_sound, ASK_GPIO_SOUND, hadc1, 4);


