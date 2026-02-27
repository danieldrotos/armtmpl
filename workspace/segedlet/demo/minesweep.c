#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "brd.h"
#include "terminal.h"
#include "mos.h"

#define TF table[y][x]
#define CF table[curs_y][curs_x]

struct field_t {
  int bomb;
  int show;
  int pin;
  int nr;
};

struct field_t table[10][10];
int curs_x, curs_y;
int population= 15;
int base_x=5, base_y=2;

enum state_t {
  st_none,
  st_game,
  st_result
};

static enum state_t state= st_none;


static void end()
{
  state= st_none;
  TU_BW;
  tu_clear_screen();
  mos_exit();
}

static int has_bomb(int x, int y)
{
  if (x<0) return 0;
  if (x>9) return 0;
  if (y<0) return 0;
  if (y>9) return 0;
  return (TF.bomb)?1:0;
}

static void start_game()
{
  int x, y, n;
  tu_fgbg(RGB_WHITE, RGB_BLACK);
  tu_clear_screen();
  tu_hide();
  for (y=0; y<10; y++)
    for (x=0; x<10; x++)
      {
        TF.bomb= 0;
        TF.pin= 0;
        TF.show= 0;
      }
  n= (10*10*population)/100;
  for ( ; n; n--)
    {
      int x,y;
      do
        {
          x= rand()%10;
          y= rand()%10;
        }
      while (TF.bomb);
      TF.bomb= 1;
    }
  for (y=0; y<10; y++)
    for (x=0; x<10; x++)
      {
        int sum= 0;
        sum+= has_bomb(x-1,y-1);
        sum+= has_bomb(  x,y-1);
        sum+= has_bomb(x+1,y-1);
        sum+= has_bomb(x-1,  y);
        sum+= has_bomb(x+1,  y);
        sum+= has_bomb(x-1,y+1);
        sum+= has_bomb(  x,y+1);
        sum+= has_bomb(x+1,y+1);
        TF.nr= sum;
      }
  for (y=0; y<10; y++)
    for (x=0; x<10; x++)
      {
        tu_go(base_x+(x*3)+1, base_y+y);
        printf("-");
      }
  curs_x= 5;
  curs_y= 5;
  tu_go(base_x+(curs_x*3), base_y+curs_y);
  printf("<");
  tu_go(base_x+(curs_x*3)+2, base_y+curs_y);
  printf(">");
  state= st_game;
}

static void start_result()
{
  int x, y;
  tu_go(1,24);
  TU_BW;
  printf("Game over");
  for (y=0; y<10; y++)
    for (x=0; x<10; x++)
      {
        tu_go(base_x+(x*3)+1, base_y+y);
        if (TF.bomb)
          printf("o");
        /*else if (TF.show)
          ;*/
        /*else if (TF.nr == 0)
          printf("-");*/
        /*else
          printf(" ");*/
      }
  state= st_result;
}

static void curs_move(int dx, int dy)
{
  int nx, ny;
  nx= curs_x+dx;
  ny= curs_y+dy;
  if (nx<0) return;
  if (nx>9) return;
  if (ny<0) return;
  if (ny>9) return;
  tu_go(base_x+(curs_x*3), base_y+curs_y);
  printf(" ");
  tu_go(base_x+(curs_x*3)+2, base_y+curs_y);
  printf(" ");
  curs_x= nx;
  curs_y= ny;
  TU_BW;
  tu_go(base_x+(curs_x*3), base_y+curs_y);
  printf("<");
  tu_go(base_x+(curs_x*3)+2, base_y+curs_y);
  printf(">");
}

static void show_field()
{
  if (CF.show)
    {

    }
  else
    {
      if (has_bomb(curs_x, curs_y))
        {
          start_result();
        }
      else
        {
          CF.show= 1;
          tu_go(base_x+(curs_x*3)+1, base_y+curs_y);
          if (CF.nr == 0)
            {
              printf(" ");
            }
          else
            {
              switch (CF.nr)
              {
                case 1: tu_fg(RGB_BLUE); break;
                case 2: tu_fg(RGB_GREEN); break;
                case 3: tu_fg(RGB_RED); break;
                case 4: tu_fg(RGB_BBLUE); break;
                case 5: tu_fg(RGB_BRED); break;
                case 6: tu_fg(RGB_CYAN); break;
                case 7: tu_fg(RGB_WHITE); break;
                case 8: tu_fg(RGB_BWHITE); break;
              }
              printf("%d", CF.nr);
            }
        }
    }
}

static void play_game()
{
  int c;
  c= tu_getc();
  if (c)
    {
      switch (c)
      {
        case TU_UP   : curs_move( 0,-1); break;
        case TU_DOWN : curs_move( 0,+1); break;
        case TU_LEFT : curs_move(-1, 0); break;
        case TU_RIGHT: curs_move(+1, 0); break;
        case ' '     : show_field(); break;
        case 'f': case 'F': case 'p': case 'P':
          if (!CF.show)
            {
              CF.pin= !CF.pin;
              tu_go(base_x+(curs_x*3)+1, base_y+curs_y);
              if (CF.pin)
                printf("#");
              else
                printf("-");
            }
          break;
        case 'q': end(); break;
      }
    }
}

void mine_loop()
{
  int c;
  switch (state)
  {
    case st_none: start_game(); break;
    case st_game: play_game(); break;
    case st_result:
      c= tu_getc();
      if (c == ' ')
        start_game();
      break;
  }
}
