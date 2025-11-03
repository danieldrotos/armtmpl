#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>

#include "brd.h"
#include "terminal.h"
#include "mos.h"
#include "utils.h"


enum {
  tps= 1000,
  bnr= 20,
  snr= 20
};

struct bull_t {
  int x, y, color;
  unsigned int last;
};

struct ship_t {
  int id, x, y, color, hits, value, type;
  unsigned int last;
};

struct row_t {
  int dir, y, steps;
  unsigned int last;
};

struct result_t {
  int point;
  int miss;
};

static int pos;
static int auto_move;
static struct result_t result;
static struct bull_t bulls[bnr];
static struct ship_t ships[snr];
static struct row_t  rows[25];
static int nuof_bulls, nuof_ships;
static int shoot_run;
static unsigned int last_down, next_down;
static int game_over;
static int bull_time= 100, down_min= 5000, down_max= 10000;
static int shift_time= 2000;

static void
status()
{
  int d;
  tu_hide();
  tu_color(1, 7);
  //tu_save_cursor();
  tu_go(1, 1);
  d= (next_down-msnow());
  if (d<0) d= -2;
  if (game_over) d= -1;
  printf("%c p=%2d b=%2d s=%2d d=%+6d ",
         auto_move?'*':' ',
         pos, nuof_bulls, nuof_ships, d);
  tu_go(50, 1);
  printf("P=%d M=%d ", result.point, result.miss);
}

static void
row_stat(int r)
{
  return;
  if (r<2)
    return;
  tu_go(1, r);
  tu_color(0,7);
  printf("%c%d", (rows[r].dir>0)?'+':'-',
         rows[r].steps);
}

static void
show_ship(struct ship_t *s)
{
  if (s->type)
    {
      tu_go(s->x, s->y);
      tu_color(0, s->color);
      switch (s->type)
      {
        case 1:	printf(" /-%c-\\ ", 'A'+s->id); break;
      }
    }
}

static void
remove_ship(struct ship_t *s)
{
  if (s->type == 0)
    return;
  tu_go(s->x, s->y);
  tu_color(0,7);
  printf("       ");
  s->type= 0;
  nuof_ships--;
  status();
}

static void
move_ship(struct ship_t *s, int dirx, int diry)
{
  int nx, ny;
  if (!(s->type))
    return;
  nx= s->x+dirx;
  ny= s->y+diry;
  if (ny == 24)
    {
      // check break;
      if ((nx+1>=pos-5) && (nx+5<=pos+4))
        {
          game_over= 1;
          status();
          return;
        }
      result.miss++;
      remove_ship(s);
    }
  if (diry==0)
    {
      s->x= nx;
      s->y= ny;
      show_ship(s);
    }
  else
    {
      tu_go(s->x, s->y);
      tu_color(0,7);
      printf("       ");
      s->x= nx;
      s->y= ny;
      show_ship(s);
    }
}

static void
move_down()
{
  int i;
  for (i=24; i>=0; i--)
    {
      rows[i+1].dir= rows[i].dir;
      rows[i+1].steps= rows[i].steps;
      rows[i+1].last= rows[i].last;
    }
  for (i=0; i<snr; i++)
    {
      move_ship(&ships[i], 0, +1);
    }
}

static void
gen_ships()
{
  int i;
  rows[2].steps= rand()%5;
  i= 0;
  while ((i<4) && (nuof_ships<snr))
    {
      int s;
      for (s=0; ships[s].type && (s<snr); s++) ;
      if (s>=snr)
        return;
      ships[s].x= 20+i*10;
      ships[s].y= 2;
      ships[s].color= 1+rand()%6;
      ships[s].hits= 0;
      ships[s].last= 0;
      ships[s].type= 1;
      ships[s].value= 5;
      show_ship(&ships[s]);
      i++;
      nuof_ships++;
      row_stat(2);
    }
}

static void
move_ships()
{
  int r;
  unsigned int now= msnow();
  if (now > next_down)
    {
      move_down();
      gen_ships();
      last_down= now;
      next_down= now + down_min + rand()%(down_max-down_min);
      return;
    }
  for (r=1; r<25; r++)
    {
      int i;
      if (now - rows[r].last > shift_time)
        {
          rows[r].last= now;
          if (rows[r].steps > 5)
            {
              rows[r].dir= (rows[r].dir>0)?-1:+1;
              rows[r].steps= 0;
              row_stat(r);
            }
          else
            {
              rows[r].steps++;
              row_stat(r);
              for (i= 0; i<snr; i++)
                {
                  if (ships[i].y == r)
                    {
                      move_ship(&ships[i], rows[r].dir, 0);
                    }
                }
            }
        }
    }
}

static void
show_bull(struct bull_t *b)
{
  if (b->y > 1)
    {
      tu_go(b->x, b->y);
      tu_color(0, b->color);
      printf("O");
    }
  b->last= /*clk*/msnow();
}

static void
remove_bull(struct bull_t *b)
{
  if (b == NULL)
    return;
  tu_go(b->x, b->y);
  tu_color(0,7);
  printf(" ");
  b->x= 0;
  b->y= 0;
  nuof_bulls--;
}

static void
check_hit(struct bull_t *b)
{
  int i;
  for (i=0; i<snr; i++)
    {
      if (ships[i].type && (b->y == ships[i].y))
        {
          if ((b->x >= ships[i].x+1) && (b->x <= ships[i].x+5))
            {
              result.point+= ships[i].value;
              remove_ship(&ships[i]);
              remove_bull(b);
            }
        }
    }
}

static int
move_bull(struct bull_t *b)
{
  tu_go(b->x, b->y);
  tu_color(0, 0);
  printf(" ");
  if ((--(b->y)) < 2)
    return 1;
  check_hit(b);
  show_bull(b);
  return 0;
}

static void
move_bulls()
{
  struct bull_t *b;
  int i;
  unsigned int now= msnow();
  for (i=0; i<bnr; i++)
  {
    b= &bulls[i];
    if  (b->x && b->y)
      {
        if ((now - b->last) > bull_time)
          {
        	b->last= now;
            if (move_bull(b))
              {
                remove_bull(b);
                status();
              }
          }
      }
  }
}

static void
show_gun()
{
  tu_go(pos, 24);
  tu_color(0, 7);
  printf("-^-");
}

static void
clear_gun()
{
  tu_go(pos, 24);
  tu_color(0, 7);
  printf("   ");
}

static void
move_gun(int diff)
{
  int n= pos+diff;
  if ((n >= 1) && (n <= 79-3))
    {
      if (n != pos)
        {
          clear_gun();
          pos= n;
          show_gun();
        }
    }
  status();
}

static void
shoot()
{
  struct bull_t *b= NULL;
  int i;

  if (nuof_bulls < bnr)
    {
      for (i=0; i<bnr; i++)
        {
          if (!bulls[i].x && !bulls[i].y)
            {
              b= &bulls[i];
              break;
            }
        }
      if (b == NULL)
        return;
      b->x= pos+1;
      b->y= 23;
      b->color= rand()%7 + 1;
      nuof_bulls++;
      show_bull(b);
    }
  status();
}

static int game_inited= 0;

void
shoot_game_init()
{
  int i;
  pos= 40;
  auto_move= 1;
  tu_process_esc(1);
  tu_raw();
  tu_hide();
  tu_color(0, 7);
  tu_clear_screen();
  tu_go(1,24);
  printf("Press user BTN or q to quit.");
  tu_go(70,1);
  printf("Quit:BTN,q");
  show_gun();
  nuof_bulls= 0;
  for (i=0;i<bnr;i++)
    bulls[i].x= bulls[i].y= 0;
  nuof_ships= 0;
  for (i=0; i<25; i++)
    {
      rows[i].dir= (rand()%100>50)?+1:-1;
      rows[i].last= 0;
      rows[i].steps= rand()%5;
      row_stat(i);
    }
  last_down= msnow();
  next_down= last_down+5000;
  for (i=0; i<snr; i++)
    {
      ships[i].type= 0;
      ships[i].id= i;
    }
  gen_ships();
  game_over= 0;
  result.miss= 0;
  result.point= 0;
  shoot_run= 1;
  game_inited= 1;
  status();
}

static void
show_state()
{
  static unsigned int last;
  unsigned int now= msnow();
  if (now - last > 200)
    {
      last= now;
      status();
    }
}

static void
finish_game()
{
  game_inited= 0;
  tu_color(0,7);
  tu_clear_screen();
  mos_exit();
}

void
shoot_game()
{
  int c;
  //if (app != app_shooter) return;
  if (!game_inited)
    shoot_game_init();
  if (auto_move)
  {
    move_bulls();
    if (!game_over) move_ships();
  }
  show_state();
  if ((c= tu_getc())!=0)
    {
      tu_go(40,1);
      tu_color(2,0);
      printf("%+5d ", c);
      switch (c)
      {
      case 'a': case 'A': case TU_LEFT:
        move_gun(-1);
        break;
      case 'd': case 'D': case TU_RIGHT:
        move_gun(+1);
        break;
      case ' ': case '\n': case '\r': case TU_UP:
        if (!game_over) shoot();
        break;
      case 'l':
        move_bulls();
        break;
      case 'p':
        tu_color(0, 7);
        tu_clear_screen();
        show_gun();
        move_bulls();
        if (!game_over) move_ships();
        status();
        break;
      case 'm': case TU_DOWN:
        auto_move= !auto_move;
        break;
      case 'b':
    	  if (bull_time<2000) bull_time+= 10;
    	  tu_go(1,24);tu_color(0,2);printf("%6d ",bull_time);
    	  break;
      case 'B':
    	  if (bull_time>10) bull_time-= 10;
    	  tu_go(1,24);tu_color(0,2);printf("%6d ",bull_time);
    	  break;
      case 'v':
    	  if (down_min<10000) down_min+= 100;
    	  down_max= down_min*2;
    	  tu_go(1,24);tu_color(0,2);printf("%6d %6d ",down_min,down_max);
    	  break;
      case 'V':
    	  if (down_min>100) down_min-= 100;
    	  down_max= down_min*2;
    	  tu_go(1,24);tu_color(0,2);printf("%6d %6d ",down_min,down_max);
    	  break;
      case 'n':
    	  if (shift_time<5000) shift_time+= 50;
    	  tu_go(1,24);tu_color(0,2);printf("%6d ",shift_time);
    	  break;
      case 'N':
    	  if (shift_time>50) shift_time-= 50;
    	  tu_go(1,24);tu_color(0,2);printf("%6d ",shift_time);
    	  break;
      case 'I'-'A'+1:
    	  shoot_game_init();
    	  break;
      case 'q':
        finish_game();
        break;
      /*case 'Q'-'A'+1: // CTRL-Q
      case 'X'-'A'+1: // CTRL-X
    	  app= app_none;
	  	  shoot_run= 0;
	  	  tu_color(0, 7);
	  	  tu_clear_screen();
	  	  tu_show();
	  	  game_inited= 0;
	  	  break;*/
      }
    }
  if (brd_button_pressed(BTN0))
    finish_game();
}
