#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "brd.h"
#include "terminal.h"
#include "utils.h"
#include "mos.h"


enum state_t {
  st_none,
  st_game,
  st_result
};

static enum state_t state= st_none;

// bat data
int bat_y;

// ball data
int ball_run;
int ball_x, ball_y;
float ball_fx, ball_fy;
int dir_x, dir_y;
float ball_angle;
int game_over;


static void end()
{
  state= st_none;
  TU_BW;
  tu_clear_screen();
  mos_exit();
}

static void show_bat()
{
  int i;
  tu_fgbg(RGB_BWHITE,RGB_BLACK);
  for (i=0; i<5; i++)
  {
      tu_go(1,i+bat_y);
      printf("|");
  }
}

static void show_ball()
{
  tu_fgbg(RGB_BYELLOW,RGB_BLACK);
  tu_go(ball_x, ball_y);
  printf("O");
}

static void show_state()
{
  tu_save_cursor();
  tu_go(1,1);
  tu_fgbg(RGB_BLACK, RGB_GREEN);
  printf("dirx=%+1d diry=%+1d ", dir_x, dir_y);
  printf("x=%6.2f y=%6.2f ", ball_fx, ball_fy);
  tu_restore_cursor();
}

static void start_game()
{
  tu_fgbg(RGB_WHITE,RGB_BLACK);
  tu_clear_screen();
  tu_hide();
  bat_y= 10;
  show_bat();
  ball_run= 0;
  ball_x= 2;
  ball_y= 12;
  ball_fx= ball_x-1;
  ball_fy= ball_y-2;
  dir_x= +1;
  dir_y= (rand()%100 > 50)?+1:-1;
  ball_angle= rand()%90;
  show_ball();
  state= st_game;
}

static void start_result(int over)
{
  game_over= over;
  printf("over=%d", game_over);
  state= st_result;
}

static void bat_up()
{
  if (bat_y > 2)
    {
      bat_y--;
      TU_BW;
      tu_go(1,bat_y+5);
      printf(" ");
      tu_go(1,bat_y);
      printf("|");
    }
}

static void bat_down()
{
  if (bat_y < 22-5+1)
    {
      bat_y++;
      TU_BW;
      tu_go(1,bat_y-1);
      printf(" ");
      tu_go(1,bat_y+4);
      printf("|");
    }
}

static void do_bat()
{
  int c;
  c= tu_getc();
  switch (c)
  {
    case 'q': start_result(0); break;
    case TU_UP: case 'w': bat_up(); break;
    case TU_DOWN: case 's': bat_down(); break;
  }
}

void recalc(float *px, float *py)
{
  float r= ball_angle * M_PI / 180.0;
  float dx, dy;
  dx= dir_x * sin(r);
  dy= dir_y * cos(r);
  *px= ball_fx+dx;
  *py= ball_fy+dy;
}

void do_ball()
{
  static unsigned int last;
  unsigned int now= msnow();
  if (now - last > 100)
    {
      int nx, ny;
      float fx, fy;
      recalc(&fx, &fy); nx= fx+1; ny= fy+2;
      if ((ny < 2) || (ny > 23))
        {
          dir_y*= -1;
          recalc(&fx, &fy); nx= fx+1; ny= fy+2;
        }
      if (nx > 80)
        {
          dir_x*= -1;
          recalc(&fx, &fy); nx= fx+1; ny= fy+2;
        }
      ball_fx= fx;
      ball_fy= fy;
      if ((nx == 2) && ((ny >= bat_y) && (ny <= bat_y+4)))
        {
          dir_x*= -1;
          switch (ny - bat_y)
          {
            case 0: ball_angle= 60; dir_y= -1; break;
            case 1: ball_angle= 30; dir_y= -1; break;
            case 2: ball_angle=  0; dir_y= +1; break;
            case 3: ball_angle= 30; dir_y= +1; break;
            case 4: ball_angle= 60; dir_y= +1; break;
          }
          //recalc(&fx, &fy); nx= fx+1; ny= fy+2;
        }
      else if (nx == 1)
        {
          start_result(1);
        }
      if ((ball_x != nx) || (ball_y != ny))
        {
          tu_fgbg(RGB_BYELLOW, RGB_BLACK);
          tu_go(ball_x, ball_y);
          printf(" ");
          tu_go(nx, ny);
          printf("O");
          ball_x= nx;
          ball_y= ny;
        }
      show_state();
      last= now;
    }
}

void do_game()
{
  if (!ball_run)
    {
      if (brd_button_pressed(BTN0))
        {
          ball_run= 1;
        }
      if (tu_getc() == 'q')
        end();
    }
  else
    {
      do_bat();
      do_ball();
    }
}

void do_result()
{
  int c;
  c= tu_getc();
  switch (c)
  {
    case ' ': start_game(); break;
    case 'q': end(); break;
  }
}

void pingpong_loop()
{
  switch (state)
  {
    case st_none: start_game(); break;
    case st_game: do_game(); break;
    case st_result: do_result(); break;
  }
}
