#include <stdio.h>
#include <stdlib.h>

#include "brd.h"
#include "terminal.h"
#include "mos.h"
#include "utils.h"

#include "flappy_bird.h"


static int bird_pos;
static int bird_time= 400;
static int wall_h[80];
static int wall_l[80];
static int wall_time= 150;
static int run;
static int inited= 0;
static int first_time;

void flappy_init()
{
  int i;
  tu_color(0,7);
  tu_clear_screen();
  tu_hide();
  printf("Press user BTN to start the game. Press q to quit.");
  bird_pos= 12;
  run= 0;
  first_time= 1;
  tu_go(10, bird_pos);
  printf("O");
  for (i=1; i<=79; i++)
    {
      wall_h[i]= 5;
      tu_go(i,wall_h[i]);
      printf("X");
      wall_l[i]= 19;
      tu_go(i,wall_l[i]);
      printf("X");
    }
  inited= 1;
}

static int collision()
{
  if (bird_pos <= wall_h[10])
    return 1;
  if (bird_pos >= wall_l[10])
    return 1;
  return 0;
}

static void bird_move(int dy)
{
  int ny= bird_pos+dy;
  if ((ny<1) || (ny>24))
    return;
  tu_go(10, bird_pos);
  printf(" ");
  bird_pos= ny;
  tu_go(10, bird_pos);
  printf("O");
  run= !collision();
}

static void bird()
{
  static unsigned int last;
  unsigned int now= msnow();
  if (now - last > bird_time)
    {
      last= now;
      bird_move(brd_button(BTN0)?-1:+1);
    }
}

static void wall_move(int *w, int ul, int ll, char c)
{
  int i, r, d, oldy, ny;
  for (i=1; i<=78; i++)
    {
      if (w[i+1] != w[i])
        {
          tu_go(i, w[i]);
          printf(" ");
          w[i]= w[i+1];
          tu_go(i, w[i]);
          printf("%c", c);
        }
    }
  d= 0;
  if ((r= rand()%100) < 10)
    d= -1;
  else if (r > 90)
    d= +1;
  if (d)
    {
      oldy= w[79];
      ny= oldy+d;
      if ((ny <= ll) && (ny >= ul) && (ny>=1) && (ny<=24))
        {
          tu_go(79, w[79]);
          printf(" ");
          w[79]= ny;
          tu_go(79, w[79]);
          printf("%c", c);
        }
    }
}

static void wall()
{
  static unsigned int last;
  unsigned int now= msnow();
  if (now - last > wall_time)
    {
      last= now;
      wall_move(wall_h,  1, wall_l[79]-5, '-');
      wall_move(wall_l, wall_h[79]+5, 24, '=');
      run= !collision();
    }
}

void flappy_loop()
{
  int c= tu_getc();
  if (!inited)
    flappy_init();
  if (run)
    {
      bird();
      wall();
    }
  else
    {
      if (first_time)
        {
          if (brd_button_pressed(BTN0))
            {
              run= 1;
              first_time= 0;
            }
        }
    }
  if (c == 'r')
    {
      if (collision())
        inited= 0;
      run= !run;
    }
  if (c == 'q')
    {
      inited= 0;
      tu_color(0,7);
      tu_clear_screen();
      mos_exit();
    }
}
