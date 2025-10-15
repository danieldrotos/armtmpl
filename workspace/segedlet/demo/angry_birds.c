#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "brd.h"
#include "terminal.h"
#include "utils.h"
#include "mos.h"


enum st_t {
  st_none   =  0,
  st_alpha  = 10,
  st_v0     = 20,
  st_setting= 33,
  st_shoot  = 43,
  st_result = 55
};

/*
    State-graph:

              +---------------------------------------+
              |    --> (v0) ---------+                | SPACE,BTN
              v   /                  v                /
(none) --> (alpha)                (shoot) --> (result)
                  \                  ^                \
            "set"  \--> (setting)----|                |
                           ^                          | ENTER
                           |                          |
                           +--------------------------+

 */

static int bottom= 40;           // last row of window
static enum st_t state;          // start state
static char buf[100];            // line ditor buffer
static float alpha, v0;          // shoot parameters
static int g= 9.81;              // phisycal constantr
static int target;               // target distance
static unsigned int t0;          // time of shoot
static int last_col, last_row;   // coords on screen
static int inited= 0;

// function prototypes
static void start_alpha();
static void start_v0();
static void start_setting();
static void start_shoot();
static void start_result();


void ab_init()
{
  // first target pre-generated
  target= 10+rand()%60;
  inited= 1;
  alpha= 45;
  v0=20;
  state= st_none;
}


void ab_finish()
{
  inited= 0;
  tu_color(0,7);
  tu_clear_screen();
  mos_exit();
}


/*
 * ALPHA state
 */

static void start_alpha()
{
  tu_color(0,7);
  tu_clear_screen();
  tu_show();
  printf("Target is at %dm\n", target);
  printf("You can switch to interactive setting mode by entering \"set\" command...\n");
  printf("Enter alpha (deg): ");
  buf[0]= 0;
  state= st_alpha;
}

/* If "set" is entered then go to setting state,
 * v0 otherwise
 */

static void do_alpha()
{
  if (tu_gets(buf, 99))
    {
      printf("\n");
      if (strcmp(buf, "set") == 0)
        start_setting();
      else if ((strcmp(buf, "exit") == 0) ||
          (strcmp(buf, "quit") == 0))
        ab_finish();
      else
        {
          alpha= strtod(buf, NULL);
          start_v0();
        }
    }
}


/*
 * V0 state
 */

static void start_v0()
{
  printf("Enter v0 (m/s): ");
  buf[0]= 0;
  state= st_v0;
}

static void do_v0()
{
  if (tu_gets(buf,99))
    {
      v0= strtod(buf, NULL);
      start_shoot();
    }
}


/*
 * SETTING state
 */

static void start_setting()
{
  //alpha= 45;
  //v0= 20;
  tu_go(1,1);
  tu_clear_screen();
  tu_hide();
  printf("Target is at %dm\n", target);
  printf("Use  UP/DOWN to set alpha, LEFT/RIGHT to set v0, ENTER to shoot:\n");
  printf("alpha= %8.2f v0= %8.2f", alpha, v0);
  state= st_setting;
}

/* Use some limits on values */

static void do_setting()
{
  int c= tu_getc();
  switch (c)
  {
    case TU_UP:    if (alpha <  85) alpha+= 1; break;
    case TU_DOWN:  if (alpha >   5) alpha-= 1; break;
    case TU_RIGHT: if (v0    < 100) v0+= 1; break;
    case TU_LEFT:  if (v0    >   5) v0-= 1; break;
    case '\n': case '\r': start_shoot(); break;
    case 'q': ab_finish(); return;
  }
  if (c)
    {
      tu_go(1,3);
      printf("alpha= %8.2f v0= %8.2f", alpha, v0);
    }
}


/*
 * SHOOT state
 */

static void start_shoot()
{
  tu_hide();
  t0= msnow();
  last_col= -1;
  last_row= -1;
  state= st_shoot;
  tu_clear_screen();
  tu_go(target, bottom);
  printf("|");
}

static void do_shoot()
{
  float t, x, y;
  int row, col, c;
  static unsigned int last;
  unsigned int now= msnow();
  // SPACE can be used to break shoot animation
  if ((c= tu_getc()) == ' ')
    {
      start_result();
      return;
    }
  if (now - last > 10)
    {
      float rad= (alpha/360.0)*2*M_PI;
      t= (float)(msnow() - t0)/1000.0;
      x= v0 * t * cos(rad);
      y= v0 * t * sin(rad) - (g*t*t/2);
      col= x+1;
      row=bottom-y;
      if ((row < 1) || (row > bottom) || (col < 1) || (col > 79))
        {
          start_result();
          return;
        }
      if ((last_col != col) || (row != last_row))
        {
          if (last_row >= 0)
            {
              tu_go(last_col, last_row);
              printf(" ");
            }
          tu_go(last_col= col, last_row= row);
          printf("*");
        }
      if ((col > 1) && (row == bottom))
        {
          start_result();
          last_col= col;
          return;
        }
      //printf("%u %u %f %f %f col=%d row=%d\n", t0, now, t, x, y, col, row);
      last= now;
    }
}


/*
 * RESULT state
 */

static void start_result()
{
  int hit;
  hit= last_col == target;
  if (hit) tu_color(2,0); else tu_color(1,7);
  tu_go(30,12); printf("             ");
  tu_go(30,13); printf("   %s  ", hit?"You won ":"You lost");
  tu_go(30,14); printf("             ");
  tu_color(4,7);
  tu_go(8,16);  printf("  Press BTN/space to restart, ENTER for interactive setting  ");
  tu_color(0,7);
  if (hit)
    target= 10+rand()%70;
  state= st_result;
}

static char *anim_str= "\\-|/-\\_+|-/\\*";
static char *anim_ptr= NULL;

static void do_result()
{
  int c;
  static unsigned int last;
  unsigned int now= msnow();
  // play some animation if fall-point is in bottom line
  if ((last_row == bottom) && (now-last > 100))
    {
      tu_color(0,1+rand()%7);
      if ((anim_ptr==NULL) || (*anim_ptr == 0)) anim_ptr= anim_str;
      tu_go(last_col, bottom); printf("%c", *anim_ptr++);
      tu_color(0,7);
      last= now;
    }
  // Two kinds of exit, depending on user action
  c= tu_getc();
  if (brd_button_pressed(BTN0) || (c == ' '))
    start_alpha();
  if ((c == '\r') || (c == '\n'))
    start_setting();
  if (c == 'q')
    ab_finish();
}


void ab_loop()
{
  if (!inited) ab_init();
  // dispatcher of application phases
  switch (state)
  {
    case st_none   : start_alpha(); break;
    case st_alpha  : do_alpha(); break;
    case st_v0     : do_v0(); break;
    case st_setting: do_setting(); break;
    case st_shoot  : do_shoot(); break;
    case st_result : do_result(); break;
  }
}
