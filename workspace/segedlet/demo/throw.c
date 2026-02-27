#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "brd.h"
#include "terminal.h"
#include "planet.h"
#include "mos.h"

#include "throw.h"


static int inited= 0;
int throw_type= 1;


void throw_init_common(int nr)
{
  int i;
  int j= 80/(nr+1);
  tu_clear_screen();
  tu_hide();
  for (i=0; planets[i].col >= 0; i++)
    {
      planets[i].col= (i+1)*j;
      tu_go(planets[i].col, planets[i].row);
      printf("@");
    }
  tu_go(1,max_row+1);
  printf("%d-----------------------------------------------------------------------------",
         throw_type);
  inited= 1;
}


void throw_init1()
{
  int i, nr= 0;
  for (i=0; planets[i].col >= 0; i++)
    {
      planets[i].v0= sqrt(2*planets[i].g*hmax);
      planets[i].t0= HAL_GetTick();
      planets[i].h= 0;
      planets[i].last_row= planets[i].row= max_row;
      nr++;
    }
  throw_init_common(nr);
}


void throw_init2()
{
  int i, nr= 0;
  float gmin= planets[0].g, v0;
  for (i=0; planets[i].col >= 0; i++)
    {
      planets[i].t0= HAL_GetTick();
      planets[i].h= 0;
      planets[i].last_row= planets[i].row= max_row;
      nr++;
      if (planets[i].g < gmin)
        gmin= planets[i].g;
    }
  v0= sqrt(2*gmin*hmax);
  for (i= 0; planets[i].col >= 0; i++)
    planets[i].v0= v0;
  throw_init_common(nr);
}


void throw_init(char *p)
{
  if (p == NULL)
    throw_type= 1;
  else
    {
      int i= strtol(p, 0, 0);
      if (i == 2)
        throw_type= 2;
      else
        throw_type= 1;
    }
  inited= 0;
}


void throw()
{
  static unsigned int last;
  int c;
  unsigned int now= HAL_GetTick();
  if (!inited)
    {
      if (throw_type == 2)
        throw_init2();
      else
        throw_init1();
    }
  if (now - last > 10)
    {
      int i;
      for (i=0; planets[i].col >= 0; i++)
        {
          struct planet_t *p= &planets[i];
          p->t= (now-p->t0)/1000.0;
          p->h= p->v0*p->t - p->g*p->t*p->t/2.0;
          (*p).row= max_row-(int)(p->h);
          if (p->h < 0)
            {
              p->t0= now;
              p->h= 0;
            }
          show(p);
        }
      last= now;
    }
  if (brd_button_pressed(BTN0) ||
      ((c= tu_getc()) == 'q'))
    {
      inited= 0;
      tu_clear_screen();
      mos_exit();
    }
}
