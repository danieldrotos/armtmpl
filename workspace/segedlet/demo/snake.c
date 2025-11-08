#include <stdio.h>
#include <stdlib.h>

#include "brd.h"
#include "terminal.h"
#include "mos.h"


struct snake_point_t {
	int x;
	int y;
};

enum {
	max_len= 100
};

static struct snake_point_t snake[max_len];

static int run;
static int snake_len;
static int dx, dy;
static float speed= 5.0;
static int applex, appley;
static int points= 0;

static void gen_apple()
{
  int x, y, i, ok= 0;
  while (!ok)
    {
      x= 1+rand()%80;
      y= 2+rand()%21;
      ok= 1;
      for (i= 0; i<snake_len; i++)
        if ((snake[i].x == x) && (snake[i].y == y))
          {
            ok= 0;
            break;
          }
    }
  applex= x;
  appley= y;
  tu_bgfg(RGB_BLACK, RGB_BRED);
  tu_go(applex, appley);
  printf("O");
}

static void draw_stat()
{
  tu_go(1,1);
  tu_bgfg(RGB_BLACK, RGB_YELLOW);
  printf("dx=%+d,dy=%+d speed=%4.1fHz ", dx, dy, speed);
  tu_fg(RGB_BWHITE);
  printf("Points=%5d", points);
}

static int snake_apple(int newx, int newy)
{
  int i;
  if ((newx == applex) && (newy == appley))
    {
      points+= 1;
      tu_bgfg(RGB_BLACK, RGB_WHITE);
      tu_go(snake[0].x, snake[0].y);
      printf("*");
      tu_go(newx, newy);
      printf("@");
      for (i= snake_len-1; i>=0; i--)
        {
          snake[i+1].x= snake[i].x;
          snake[i+1].y= snake[i].y;
        }
      snake[0].x= newx;
      snake[0].y= newy;
      if (snake_len < max_len)
        {
          if (++snake_len == max_len)
            {
              run= 0;
              // WIN!
              tu_fgbg(RGB_BWHITE, RGB_GREEN);
              tu_go(34,11); printf("            ");
              tu_go(34,12); printf("  YOU WON   ");
              tu_go(34,13); printf("            ");
            }
        }
      draw_stat();
      gen_apple();
      return 1;
    }
  return 0;
}

int snake_collide(int newx, int newy)
{
  int i;
  for (i= 0; i < snake_len; i++)
    {
      if ((snake[i].x == newx) && (snake[i].y == newy))
        {
          run= 0;
          tu_fgbg(RGB_BLACK, RGB_BRED);
          tu_go(34,11); printf("            ");
          tu_go(34,12); printf("  YOU LOST  ");
          tu_go(34,13); printf("            ");
          return 1;
        }
    }
  return 0;
}

void snake_move()
{
  static unsigned int last;
  unsigned int now= HAL_GetTick();
  unsigned int dt= now - last;
  int p= (1.0/speed)*1000.0;
  int newx, newy, i;
  if (dt > p)
    {
      last= now;
      newx= snake[0].x + dx;
      newy= snake[0].y + dy;
      if (newx <  1) newx= 80;
      if (newx > 80) newx=  1;
      if (newy <  2) newy= 23;
      if (newy > 23) newy=  2;
      // alma
      if (snake_apple(newx, newy))
        return;
      // utkozes
      if (snake_collide(newx, newy))
        return;
      tu_bgfg(RGB_BLACK, RGB_WHITE);
      tu_go(snake[snake_len-1].x, snake[snake_len-1].y);
      printf(" ");
      tu_go(snake[0].x, snake[0].y);
      printf("*");
      tu_go(newx, newy);
      printf("@");

      for (i= snake_len-2; i>=0; i--)
        {
          snake[i+1].x= snake[i].x;
          snake[i+1].y= snake[i].y;
        }
      snake[0].x= newx;
      snake[0].y= newy;
    }
}

void snake_init(char *param)
{
  int i;
  dx= 1;
  dy= 0;
  snake_len= 3;
  snake[0].x= 40;
  snake[0].y= 11;
  snake[1].x= 39;
  snake[1].y= 11;
  snake[2].x= 38;
  snake[2].y= 11;
  tu_bgfg(RGB_BLACK, RGB_WHITE);
  tu_clear_screen();
  tu_hide();

  tu_go(snake[0].x, snake[0].y);
  printf("@");
  for (i= 1; i<snake_len; i++)
    {
      tu_go(snake[i].x, snake[i].y);
      printf("*");
    }
  draw_stat();
  gen_apple();
  run= 1;
}

void snake_loop()
{
  int c;
  if ( ( c= tu_getc() ) )
    {
      switch (c)
        {
        case 'q': mos_exit(); break;
        case TU_LEFT:  case 'a': dx= -1; dy=  0; draw_stat(); break;
        case TU_RIGHT: case 'd': dx= +1; dy=  0; draw_stat(); break;
        case TU_UP:    case 'w': dx=  0; dy= -1; draw_stat(); break;
        case TU_DOWN:  case 's': dx=  0; dy= +1; draw_stat(); break;
        case ' ':
          if (!run)
            snake_init(NULL);
          break;
        }
    }
  if (run)
    snake_move();
}
