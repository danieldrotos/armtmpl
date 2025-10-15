#ifndef SEGEDLET_HEADER
#define SEGEDLET_HEADER

#include <stdint.h>

#include "xhw_types.h"
#include "xhw_memmap.h"
#include "xhw_tpm.h"
#include "xcore.h"
#include "xsysctl.h"
#include "xgpio.h"
#include "xwdt.h"
#include "xuart.h"

enum clock_t {
  CLOCK= 48000000
};

extern volatile unsigned int clk;
extern unsigned int HAL_GetTick();

// BOARD
extern void board_init();
extern void systick_init(float time);

// LED
extern void red_set(int v);
extern void red_on();
extern void red_off();
extern void red_toggle();

extern void green_set(int v);
extern void green_on();
extern void green_off();
extern void green_toggle();

extern void blue_set(int v);
extern void blue_on();
extern void blue_off();
extern void blue_toggle();
extern int  blue_pwm(float fr, int duty);
extern void blue_light(int percent);
extern int light2duty(int light);

// UART
extern void uart_init();
extern int uart_avail();
extern int uart_echo;
extern int uart_read(char *ptr, int len);
extern int uart_write(char *ptr, int len);


// TSI
extern int tsi_value, tsi_raw,  tsi_prev_on, tsi_prev_off;
extern void tsi_init();
extern void tsi_start();
extern int tsi_progress();
extern int tsi_done();
extern int tsi_get();
extern int tsi_scan();
extern void tsi_loop();
extern int tsi_touch();
extern int tsi_touched();
extern int tsi_released();

#endif
