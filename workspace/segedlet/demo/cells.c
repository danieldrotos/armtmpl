#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "mos.h"
#include "terminal.h"
#include "brd.h"


#define XS 79
#define YS 22


static int cells, cyc= 0, run= 1, inited= 0;
static int ctime= 500;

static char act[XS][YS];
static char next[XS][YS];
static char sum[XS][YS];


static int is_cell(int x, int y)
{
  return (act[x][y]==' ')?0:1;
}

static void end()
{
  tu_clear_screen();
  inited= 0;
  mos_exit();
}

static int tinit()
{
  int x, y, cells= 0;
  for (y=0; y<YS; y++)
    for (x=0; x<XS; x++)
      {
	act[x][y]= (rand()%100 < 20)?'*':' ';
	cells+= is_cell(x, y);
      }
  return cells;
}

static int summa(int x, int y)
{
  int i, j, s= (act[x][y]!=' ')?-1:0, d;
  for (j= y-1; j<=y+1; j++)
    for (i= x-1; i<=x+1; i++)
      {
	if ((i>=0) && (j>=0) && (i<XS) && (j<YS))
	  {
	    d= is_cell(i, j);
	    s+= d;
	  }
      }
  return s;
}

static int count()
{
  // t -->>> uj
  int x, y, cells= 0;
  for (y=0;y<YS;y++)
    for (x=0;x<XS;x++)
      {
	int s= summa(x, y);
	sum[x][y]= '0'+s;
	next[x][y]= (act[x][y]==' ')?' ':('0'+s);
	if ((s<2) || (s>3))
	  next[x][y]= ' ';
	else if (s==3)
	  next[x][y]= '0'+s;
	cells+= (next[x][y]==' ')?0:1;
      }
  return cells;
}

static void draw(int xtop, int ytop, char tab[XS][YS], char c)
{
  int x, y;
  for (y=0; y<YS; y++)
    {
      tu_go(xtop, ytop+y);
      for (x=0; x<XS; x++)
	{
	  printf("%c", (tab[x][y]==' ')?' ':(c?c:tab[x][y]));
	}
      fflush(0);
    }
}

static void draw2(int topx, int topy)
{
  int x, y;
  for (y=0;y<YS;y++)
    {
      tu_go(topx, topy+y);
      for (x=0;x<XS;x++)
	{
	  if (act[x][y] != next[x][y])
	    {
	      printf("%c", next[x][y]);
	      fflush(0);
	    }
	}
    }
}

static void step_done()
{
  // next -->> act
  int x, y;
  for (y=0; y<YS; y++)
    for (x=0; x<XS; x++)
      {
	act[x][y]= (next[x][y]==' ')?' ':'*';
      }
}

static void print()
{
  //tu_go(1,YS+3);
  int x, y;
  for (y=0;y<YS;y++)
    {
      printf("\n");
      for (x=0;x<XS;x++)
	printf("%2d %2d %c= %c -> %c\n",x,y,act[x][y],sum[x][y],next[x][y]);
    }
}

static void status()
{
  tu_go(1,1);printf("%10d %3d%%", cyc, cells*100/(XS*YS));fflush(0);
  tu_go(1,24);printf("ctime=%5u", ctime);fflush(0);
  printf(" %s", run?"RUN ":"stop");
  tu_go(19,24);
  printf("R Restart i Inject +/- Speed SPC Start/Stop CR Step q quit");
  fflush(0);
}

static void refresh()
{
  status();
  draw(1,2, act, '@');
}

static void step()
{
  cells= count();
  refresh();
  step_done();
  //draw2(1,2);
  //draw(27,2, sum, 0);
  //draw(52,2, next, 0);
  //printf("\n");
  //print();
  //exit(0);
  cyc++;
}


static void sim()
{
  static unsigned int last;
  unsigned int now= HAL_GetTick();
  if (now-last > ctime)
    {
      last= now;
      if (run)
	step();
    }
}

static void injekt()
{
  int i;
  for (i=0; i<XS*YS/10; i++)
    {
      int x, y;
      x= rand()%XS;
      y= rand()%YS;
      act[x][y]= '*';
    }
  refresh();
}

static void cells_init()
{
  if (!inited)
    {
      tu_color(0,7);
      tu_hide();
      tu_clear_screen();
      cells= tinit();
      refresh();
      inited= 1;
    }
}

static void ui()
{
  int c;
  if (!(c= tu_getc()))
    return;

  switch (c)
    {
    case 'R':
      cells= tinit();
      cyc= 0;
      tu_clear_screen();
      break;
    case '+':
      if (ctime > 10)
	ctime-= 100;
      if (ctime < 10)
	ctime= 10;
      status();
      break;
    case '-':
      if (ctime < 5000)
	ctime+= 100;
      if (ctime > 5000)
	ctime= 5000;
      status();
      break;
    case ' ':
      run= !run;
      status();
      break;
    case '\r': case '\n':
      if (!run)
	step();
      break;
    case 'i':
      injekt();
      break;
    case 'L'-'A'+1: // Ctrl-L
      refresh();
      break;
    case 'q':
      end();
      break;
    }
}

void cells_loop(void)
{
  cells_init();
  ui();
  sim();
}
