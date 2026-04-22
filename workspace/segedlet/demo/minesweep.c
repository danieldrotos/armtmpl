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
int base_x=5, base_y=3;
int size_x= 10, size_y= 10;

enum state_t {
  st_none,
  st_game,
  st_over,
  st_win
};

static enum state_t state= st_none;


static void end()
{
  state= st_none;
  TU_BW;
  tu_clear_screen();
  mos_exit();
}


static int onboard(int x, int y)
{
  if ((x < 0) || (y < 0))
    return 0;
  if ((x >= size_x) || (y >= size_y))
    return 0;
  return 1;
}

static int field_color(int nr)
{
  switch (nr)
    {
    case 1: return(RGB_BLUE); break;
    case 2: return(RGB_GREEN); break;
    case 3: return(RGB_RED); break;
    case 4: return(RGB_BBLUE); break;
    case 5: return(RGB_BRED); break;
    case 6: return(RGB_CYAN); break;
    case 7: return(RGB_WHITE); break;
    case 8: return(RGB_BWHITE); break;
    }
  return RGB_WHITE;
}

static int has_bomb(int x, int y)
{
  if (x<0) return 0;
  if (x>9) return 0;
  if (y<0) return 0;
  if (y>9) return 0;
  return (TF.bomb)?1:0;
}

static int has_pin(int x, int y)
{
  if (x<0) return 0;
  if (x>9) return 0;
  if (y<0) return 0;
  if (y>9) return 0;
  return (TF.pin)?1:0;
}

static void draw_cursor()
{
  TU_BW;
  tu_go(base_x+(curs_x*3), base_y+curs_y);
  printf("<");
  tu_go(base_x+(curs_x*3)+2, base_y+curs_y);
  printf(">");
}

static void draw_field(int x, int y)
{
  tu_go(base_x+(x*3)+1, base_y+y);
  if (!TF.show)
    TU_BW, printf("-");
  else
    {
      if (TF.nr == 0)
	printf(" ");
      else
	{
	  tu_fg(field_color(TF.nr));
	  printf("%d", TF.nr);
	}
    }
}

static void draw_table()
{
  int x, y;
  for (y=0; y<size_y; y++)
    for (x=0; x<size_x; x++)
      draw_field(x, y);
}

static void redraw()
{
  draw_table();
  draw_cursor();
}

static int pined_neighbors(int x, int y)
{
  int sum= 0;
  sum+= has_pin(x-1,y-1);
  sum+= has_pin(  x,y-1);
  sum+= has_pin(x+1,y-1);
  sum+= has_pin(x-1,  y);
  sum+= has_pin(x+1,  y);
  sum+= has_pin(x-1,y+1);
  sum+= has_pin(  x,y+1);
  sum+= has_pin(x+1,y+1);
  return sum;
}

static void start_game()
{
  int x, y, n;
  tu_fgbg(RGB_WHITE, RGB_BLACK);
  tu_clear_screen();
  tu_hide();
  for (y=0; y<size_y; y++)
    for (x=0; x<size_x; x++)
      {
        TF.bomb= 0;
        TF.pin= 0;
        TF.show= 0;
      }
  n= (size_x*size_y*population)/100;
  for ( ; n; n--)
    {
      int x,y;
      do
        {
          x= rand()%size_x;
          y= rand()%size_y;
        }
      while (TF.bomb);
      TF.bomb= 1;
    }
  for (y=0; y<size_y; y++)
    for (x=0; x<size_x; x++)
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
  draw_table();
  curs_x= 5;
  curs_y= 5;
  draw_cursor();
  state= st_game;
}

static int win()
{
  int x, y;
  for (y=0; y<size_y; y++)
    for (x=0; x<size_x; x++)
      if ((TF.bomb && !TF.pin) ||
	  (!TF.bomb && !TF.show))
	return 0;
  return 1;
}

static void start_over()
{
  int x, y;
  tu_go(1,24);
  TU_BW;
  printf("Game over");
  for (y=0; y<size_y; y++)
    for (x=0; x<size_x; x++)
      {
        tu_go(base_x+(x*3)+1, base_y+y);
        if (TF.bomb)
          printf("o");
      }
  state= st_over;
}

static void start_win()
{
  tu_go(1,24);
  tu_fg(RGB_BGREEN);
  printf("You WON!");
  state= st_win;
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

static void flip_all(int x, int y);

static void flip_field(int x, int y, int recursed)
{
  if (!onboard(x, y))
    return;
  
  if (state != st_game)
    return;
  
  if (TF.show)
    {
      if (TF.nr && (TF.nr == pined_neighbors(x, y)) && !recursed)
	flip_all(x, y);
      return;
    }

  if (TF.pin)
    return;
  
  if (has_bomb(x, y))
    {
      start_over();
      return;
    }

  TF.show= 1;
  draw_field(x, y);
  if (TF.nr == 0)
    flip_all(x, y);
}

static void flip_all(int x, int y)
{
  if (state == st_game) flip_field(x-1,y-1, 1);
  if (state == st_game) flip_field(x  ,y-1, 1);
  if (state == st_game) flip_field(x+1,y-1, 1);
  if (state == st_game) flip_field(x-1,y  , 1);
  if (state == st_game) flip_field(x+1,y  , 1);
  if (state == st_game) flip_field(x-1,y+1, 1);
  if (state == st_game) flip_field(x  ,y+1, 1);
  if (state == st_game) flip_field(x+1,y+1, 1);
}

static void play_game()
{
  int c;
  c= tu_getc();
  if (c)
    {
      switch (c)
	{
	case TU_UP    : curs_move( 0,-1); break;
	case TU_DOWN  : curs_move( 0,+1); break;
        case TU_LEFT  : curs_move(-1, 0); break;
        case TU_RIGHT : curs_move(+1, 0); break;
        case ' '      :
	  flip_field(curs_x, curs_y, 0);
	  if (CF.show && !CF.bomb && win())
	    start_win();
	  break;
	case 'L'-'A'+1: redraw(); break;
        case 'f': case 'F': case 'p': case 'P':
          if (!CF.show)
            {
              CF.pin= !CF.pin;
              tu_go(base_x+(curs_x*3)+1, base_y+curs_y);
              if (CF.pin)
                printf("#");
              else
                printf("-");
	      if (CF.pin && win())
		start_win();
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
    case st_over:
    case st_win:
      c= tu_getc();
      switch (c)
	{
        case TU_UP   : curs_move( 0,-1); break;
        case TU_DOWN : curs_move( 0,+1); break;
        case TU_LEFT : curs_move(-1, 0); break;
        case TU_RIGHT: curs_move(+1, 0); break;
	case ' '     : start_game(); break;
	case 'q'     : end(); break;
	}
      break;
    }
}
