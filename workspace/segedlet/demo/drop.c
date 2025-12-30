#include <stdio.h>
#include "brd.h"
#include "terminal.h"

#include "planet.h"
#include "mos.h"

#include "drop.h"


static enum drop_state_t state= ds_none;


void drop_init()
{
  int i, nr= 0, j;
  tu_clear_screen();
  tu_hide();
  for (i=0; planets[i].col >= 0; i++)
    {
      planets[i].t0= HAL_GetTick();
      planets[i].h= 0;
      planets[i].last_row= planets[i].row= 2;
      nr++;
    }
  j= 80/(nr+1);
  for (i=0; planets[i].col >= 0; i++)
    {
      planets[i].col= (i+1)*j;
      tu_go(planets[i].col, planets[i].row);
      printf("@");
      tu_go(planets[i].col-1, 1);
      printf("%s", planets[i].title);
    }
  tu_go(1,max_row+1);
  printf("------------------------------------------------------------------------------");
  state= ds_run;
}


void drop_step()
{
  int i;
  static unsigned int last;
  unsigned int now= HAL_GetTick();
  if (now - last > 10)
    {
      int running= 0;
      for (i=0; planets[i].col >= 0; i++)
        {
          struct planet_t *p= &planets[i];
          if (p->row < max_row)
            {
              p->t= (now-p->t0)/1000.0;
              p->h= p->g*p->t*p->t/2.0;
              (*p).row= (int)(p->h)+2;
              running= 1;
              show(p);
            }
        }
      if (!running)
        state= ds_paused;
      last= now;
    }
  if ((state == ds_run) && (0/*pause button*/))
    state= ds_paused;
}


void drop()
{
  int c;
  switch (state)
  {
    case ds_none: drop_init(); break;
    case ds_run: drop_step(); break;
    case ds_paused:
      if (1/*resume button*/)
        state= ds_run;
      break;
    case ds_done:
      if (0/*restart button*/)
        drop_init();
      break;
  }
  if (brd_button_pressed(BTN0) ||
      ((c= tu_getc())=='q'))
    {
      state= ds_none;
      tu_clear_screen();
      mos_exit();
    }
}
