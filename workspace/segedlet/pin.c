#include "utils.h"

#include "dev_gpio.h"
#include "dev_timer.h"
#include "dev_adc.h"

#include "pin.h"


int pin_init(struct pin_t *p)
{
  if (!p) return 1;
  if (p->pinmask == 0)
    {
      if ((p->name == NULL) || (*(p->name) == 0)) return 1;
      if ((p->devnr= dev_gpio_devnr_of(p->name)) >= 0)
        {
          p->pinnr= dev_gpio_pinnr_of(p->name);
          p->pinmask= dev_gpio_pinmask_of(p->name);
          p->oband= dev_gpio_band_output(p->name);
          p->iband= dev_gpio_band_input(p->name);
          p->frequ= 200;
        }
    }
  return 0;
}

static void pin_setup(struct pin_t *p, enum pin_mode_t mode)
{
  if (pin_init(p)) return;
  if (p->mode != mode)
    {
      if (mode == PIN_PORT)
        {
          if (p->output)
            dev_gpio_output(p->name);
          else
            dev_gpio_input_ud(p->name, p->pupd);
        }
      else if (p->timer != NULL)
        {
          dev_gpio_af(p->name, p->af);
        }
      else if (p->adc != NULL)
        {
          dev_gpio_analog(p->name);
        }
      p->mode= mode;
    }
}

static void pin_osetup(struct pin_t *p)
{
  if (pin_init(p)) return;
  if (!p->dlock)
    p->output= 1;
  pin_setup(p, PIN_PORT);
}

static void pin_isetup(struct pin_t *p)
{
  if (pin_init(p)) return;
  if (!p->dlock)
    p->output= 0;
  pin_setup(p, PIN_PORT);
}

void pin_input(struct pin_t *p)
{
  pin_isetup(p);
}

void pin_ouput(struct pin_t *p)
{
  pin_osetup(p);
}

static void pin_asetup(struct pin_t *p)
{
  if (pin_init(p)) return;
  p->output= 0;
  pin_setup(p, PIN_AN);
}

void pin_on(struct pin_t *p)
{
  if (pin_init(p)) return;
  *p->oband= 1;
  pin_osetup(p);
}

void pin_off(struct pin_t *p)
{
  if (pin_init(p)) return;
  *p->oband= 0;
  pin_osetup(p);
}

void pin_toggle(struct pin_t *p)
{
  if (pin_init(p)) return;
  *p->oband^= 1;
  pin_osetup(p);
}

void pin_set(struct pin_t *p, int on)
{
  if (pin_init(p)) return;
  *p->oband= on?1:0;
  pin_osetup(p);
}

void pin_pwm(struct pin_t *p, float duty)
{
  if (pin_init(p)) return;
  if (p->timer != NULL)
    {
      dev_timer_pwm_start(p->timer, p->ch);
      pin_setup(p, PIN_PWM);
      dev_timer_set_duty(p->timer, p->ch, duty);
    }
  else
    {
      pin_set(p, (duty>50)?1:0);
      pin_osetup(p);
    }
}


void pin_bright(struct pin_t *p, float brightness)
{
  float d;
  d= 0.01 * brightness * brightness;
  pin_pwm(p, d);
}

void pin_freq(struct pin_t *p, int fr)
{
  if (pin_init(p)) return;
  if (p->timer != NULL)
    {
      pin_setup(p, PIN_PWM);
      p->frequ= fr;
      dev_timer_set_freq(p->timer, p->ch, fr);
    }
}


int pin_get(struct pin_t *p)
{
  if (pin_init(p)) return 0;
  if (p->adc != NULL)
    {
      pin_asetup(p);
      return dev_adc_read(p->adc, p->ch);
    }
  else if (p->output)
    {
      return *p->oband;
    }
  pin_isetup(p);
  return *p->iband;
}


int pin_iget(struct pin_t *p)
{
  if (pin_init(p)) return 0;
  return *p->iband;
}

int pin_oget(struct pin_t *p)
{
  if (pin_init(p)) return 0;
  return *p->oband;
}

int pin_ison(struct pin_t *p)
{
  p->output= 0;
  int v= pin_get(p);
  return (v == p->onvalue)?1:0;
}

int pin_val(struct pin_t *p)
{
  return p->filtered;
}

void pin_debounce(struct pin_t *p)
{
  uint now= msnow();
  if (p->last_debounce - now > 50)
    {
      p->filtered= pin_ison(p);
      p->last_debounce= now;
    }
}

int pin_edge_prepared(struct pin_t *p)
{
  if (pin_init(p)) return 0;
  pin_debounce(p);
  if (!p->edge_started && (p->last_debounce != 0))
    {
      p->press_last= p->release_last= p->filtered;
      p->edge_started= 1;
      return 0;
    }
  return 1;
}

int pin_pressed(struct pin_t *p)
{
  if (!pin_edge_prepared(p)) return 0;
  if (p->press_last != p->filtered)
    {
      p->press_last= p->filtered;
      return (p->filtered == p->onvalue)?1:0;
    }
  return 0;
}

int pin_released(struct pin_t *p)
{
  if (!pin_edge_prepared(p)) return 0;
  if (p->release_last != p->filtered)
    {
      p->release_last= p->filtered;
      return (p->filtered != p->onvalue)?1:0;
    }
  return 0;
}
