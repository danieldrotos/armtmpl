#ifndef PIN_H_
#define PIN_H_

#include <stdint.h>

#include "types.h"

enum pin_mode_t {
  PIN_NONE= 0,
  PIN_PORT= 1,
  PIN_PWM= 2,
  PIN_AN= 3
};

struct pin_t {
	char *name;             // pin name
	char *init;
        int output;             // TRUE if direction is out
        int dlock;              // direction is locked
	int devnr;              // device number (see in f4_gpio.c
	int pinnr;              // number of pin: 0-15
	int pinmask;            // mask of pin
	int pupd;               // 0=NO 1=UP 2=DOWN
        int af;                 // AF value for config
	uint32_t *oband;        // band of ODR bit
	uint32_t *iband;        // band of IDR bit
	enum pin_mode_t mode;   // port/pwm/analog
	// input related values
	int onvalue;            // value means pressed
	int filtered;           // debounced value
	uint last_debounce;     // time of last bedounce
	int edge_started;       // if edge detection inited
	int press_last;         // last value of press detect
	int release_last;       // last value of release detect
	// pwm related values
	void *timer;            // TIM_HandleTypeDef
	int ch;                 // timer/adc channel
	int frequ;              // frequ of signal
	// analog related values
	void *adc;              // ADC_HandleTypeDef
};


#define PIN_PIN(PN)          { .name=PN,.output=0,.dlock=0 }
#define PIN_OPIN(PN)         { .name=PN,.output=1,.dlock=1 }
#define PIN_PPIN(PN,T,CH,AF) { .name=PN,.output=1,.dlock=1,.timer=&T,.ch=CH,.af=AF }
#define PIN_IPIN(PN)         { .name=PN,.output=0,.dlock=1 }
#define PIN_BTN(PN,ONV)      { .name=PN,.output=0,.dlock=1,.onvalue=ONV }
#define PIN_BTNU(PN,ONV)     { .name=PN,.output=0,.dlock=1,.onvalue=ONV,.pupd=1 }
#define PIN_BTND(PN,ONV)     { .name=PN,.output=0,.dlock=1,.onvalue=ONV,.pupd=2 }
#define PIN_BTNH(PN)         { .name=PN,.output=0,.dlock=1,.onvalue=1,.pupd=2 }
#define PIN_BTNL(PN)         { .name=PN,.output=0,.dlock=1,.onvalue=0,.pupd=1 }
#define PIN_AN(PN,A,CH)      { .name=PN,.adc=&A,.ch=CH }
#define PIN_NONE             { .name=NULL }

#define DEF_PIN(N,PN)          struct pin_t N= PIN_PIN(PN)
#define DEF_OPIN(N,PN)         struct pin_t N= PIN_OPIN(PN)
#define DEF_PPIN(N,PN,T,CH,AF) struct pin_t N= PIN_PPIN(PN,T,CH,AF)
#define DEF_IPIN(N,PN)         struct pin_t N= PIN_IPIN(PN)
#define DEF_BTN(N,PN,ONV)      struct pin_t N= PIN_BTN(PN,ONV)
#define DEF_BTNU(N,PN,ONV)     struct pin_t N= PIN_BTNU(PN,ONV)
#define DEF_BTND(N,PN,ONV)     struct pin_t N= PIN_BTND(PN,ONV)
#define DEF_BTNH(N,PN)         struct pin_t N= PIN_BTNH(PN)
#define DEF_BTNL(N,PN)         struct pin_t N= PIN_BTNL(PN)
#define DEF_AN(N,PN,A,CH)      struct pin_t N= PIN_AN(PN,A,CH)


extern int  pin_init  (struct pin_t *p);
extern void pin_input (struct pin_t *p);
extern void pin_output(struct pin_t *p);

extern void pin_on    (struct pin_t *p);
extern void pin_off   (struct pin_t *p);
extern void pin_toggle(struct pin_t *p);
extern void pin_set   (struct pin_t *p, int on);

extern void pin_pwm   (struct pin_t *p, float duty);
extern void pin_bright(struct pin_t *p, float brightness);
extern void pin_freq  (struct pin_t *p, int fr);

extern int pin_get (struct pin_t *p); // actual value, dep on dir/mode
extern int pin_oget(struct pin_t *p); // ODR
extern int pin_iget(struct pin_t *p); // IDR
extern int pin_ison(struct pin_t *p); // true if equ to onvalue

#define pin_read pin_get
#define pin_write pin_set

extern void pin_debounce(struct pin_t *p);

extern int pin_val     (struct pin_t *p); // true if equ to onvalue, debounced
extern int pin_pressed (struct pin_t *p); // edge to onvalue
extern int pin_released(struct pin_t *p); // edge from onvalue


#endif /* PIN_H_ */
