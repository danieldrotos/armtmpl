#include <ask_test.h>
#include <stdio.h>
#include <stdint.h>

#include "brd.h"
#include "demo.h"
#include "mos.h"


void app_init()
{
  brd_init();
  demo_init();
  mos();
  mos_start_task("blink", "0.8");
  mos_start_task("shoot_game", NULL);
}


void app_loop()
{
  mos();
}
