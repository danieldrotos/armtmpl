#include <stdlib.h>

#include "brd.h"
#include "terminal.h"
#include "demo.h"
#include "flappy_bird.h"
#include "shoot_game.h"
#include "throw.h"
#include "drop.h"
#include "angry_birds.h"
#include "snake.h"
#include "mos.h"
#include "utils.h"


static int half_period= 500; // in ms

static void blink_start(char *param)
{
  double fr;
  if (param && *param)
    {
      fr= strtod(param, NULL);
      half_period= 1000.0/fr;
      half_period/= 2;
    }
}

static void blink()
{
  static uint32_t last, on;
  uint32_t now= msnow();
  if (now - last > half_period)
    {
      on= !on;
      //brd_led_toggle(LED0);
      brd_led_pwm(on?90:20);
      last= now;
    }
}

static void blink_stop()
{
  brd_led_off(LED0);
}

MOS_TASKIE(blink_task, "blink", &blink, &blink_start, &blink_stop);


static char set_cmd[100]= { 0 };
static int set_state= 0;

static void blink_set_start(char *param)
{
  blink_start(param);
  if (param)
    set_state= 2;
}

static void blink_set()
{
  switch (set_state)
  {
    case 0:
      printf("Enter frequ in Hz: ");
      set_state= 1;
      set_cmd[0]= 0;
      break;
    case 1:
      if (tu_gets(set_cmd, 99))
        {
          printf("\n");
          blink_start(set_cmd);
          mos_exit();
          set_state= 0;
        }
      break;
    case 2:
      set_state= 0;
      mos_exit();
      break;
  }
}

void demo_init()
{
  mos_init();
  mos_register_task(&blink_task);
  mos_create_task("blink_set", 1, &blink_set)->init= &blink_set_start;
  mos_create_task("shoot_game", 1, &shoot_game);
  mos_create_task("flappy_bird", 1, &flappy_loop);
  mos_create_task("angry_birds", 1, &ab_loop);
  mos_create_task("throw", 1, &throw)->init= &throw_init;
  mos_create_task("drop", 1, &drop);
  mos_create_task("snake", 1, &snake_loop)->init= &snake_init;
}

