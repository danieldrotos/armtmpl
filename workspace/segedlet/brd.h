#ifndef BRD_HEADER
#define BRD_HEADER

#include "dev.h"


#ifdef DEV_N410
// nucleo 410R
#define BRD_N410
#define BRD_F4
#include "brd_n410.h"

#elif defined DEV_N411
// nucleo 411R
#define BRD_N411
#define BRD_F4
#include "brd_n411.h"

#elif defined DEV_N401
// nucleo 401R
#define BRD_N401
#define BRD_F4
#include "brd_n401.h"

#elif defined DEV_N491
// nucleo 491
#define BRD_N491
#define BRD_G4
#include "brd_g491.h"

#elif defined DEV_F4DISC
// discovery 407
#define BRD_F4DISC
#define BRD_F4
#include "brd_f4disc.h"

#elif defined DEV_DOAF4
// DOA-F4
#define BRD_DOAF4
#define BRD_F4
#include "brd_doaf4.h"

#elif defined DEV_L433
// nucleo L433
#define BRD_L433
#define BRD_L4
#include "brd_l433.h"

#endif



extern unsigned int brd_leds;
extern unsigned int brd_btns;
extern unsigned int brd_buttons;
extern int brd_btn_active_state;
extern char *brd_stdio;

extern void brd_type_init();
extern void brd_init();

extern void brd_led_on(int led);
extern void brd_led_off(int led);
extern void brd_led_toggle(int led);
extern void brd_led_set(int, int on);
extern int  brd_led_get(int led);
extern void brd_led_pwm(float power);

extern void brd_sample_buttons();
extern unsigned int brd_button(int btn);
extern int brd_button_pressed(int btn);
extern int brd_button_released(int btn);

enum brd_lmode_t {
	BRD_LMODE_NONE= 0,
	BRD_LMODE_LED= 1,
	BRD_LMODE_PWM= 2
};

#include "pin.h"
#include "tim.h"

/*
enum {
  brd_nuof_leds= 8,
  brd_nuof_btns= 4
};
*/

extern struct pin_t brd_pins[];
//extern struct pin_t brd_btn_pins[];

#endif /* BRD_HEADER */
