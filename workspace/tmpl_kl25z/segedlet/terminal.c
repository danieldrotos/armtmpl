#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "terminal.h"
//#include "vcp.h"
//#include "ols_fs.h"
#include "segedlet.h"

int tu_raw_mode= 0;
int do_process_esc= 1;

void tu_process_esc(int val)
{
	do_process_esc= val;
}

static int tu_buffer[tu_buf_size];
static int tu_first_free= 0, tu_last_used= 0;

static void
tu_put(int c)
{
  int new= (tu_first_free+1)%tu_buf_size;
  if (new == tu_last_used)
    return;
  tu_buffer[tu_first_free]= c;
  tu_first_free= new;
}

static int
tu_get()
{
  int c= tu_buffer[tu_last_used];
  if (tu_first_free == tu_last_used)
    return 0;
  tu_last_used= (tu_last_used+1)%tu_buf_size;
  return c;
}

static char tu_esc_buffer[100]= { 0 };

static int
tu_proc_esc(char c)
{
  if (tu_esc_buffer[0] == '\033')
    {
      int l= strlen(tu_esc_buffer);
      tu_esc_buffer[l]= c;
      l++;
      tu_esc_buffer[l]= 0;
      if ((l > 2) &&
	  tu_esc_buffer[2] == 'M')
	{
	  // Mouse
	  if (l == 6)
	    {
	      int /*f,*/ x, y, ret= 0;
	      switch (tu_esc_buffer[3])
		{
		case ' ': ret= TU_BTN1; break;
		case '!': ret= TU_BTN2; break;
		case '"': ret= TU_BTN3; break;
		case '0': ret= TU_CBTN1; break;
		case '1': ret= TU_CBTN2; break;
		case '2': ret= TU_CBTN3; break;
		case '(': ret= TU_ABTN1; break;
		case ')': ret= TU_ABTN2; break;
		case '*': ret= TU_ABTN3; break;
		case '`': ret= TU_SUP; break;
		case 'a': ret= TU_SDOWN; break;
		case 'p': ret= TU_CSUP; break;
		case 'q': ret= TU_CSDOWN; break;
		}
	      //f= ret;
	      ret&= 0xffff00;
	      x= (tu_esc_buffer[4] - 0x20) & 0xff;
	      y= (tu_esc_buffer[5] - 0x20) & 0xff;
	      ret|= x << 16;
	      ret|= y << 8;
	      tu_esc_buffer[0]= 0;
	      return ret;
	    }
	}
      if (tu_esc_buffer[1] != '[')
	{
	  tu_esc_buffer[0]= 0;
	  return c;
	}
      if (isalpha((int)c))
	{
	  tu_esc_buffer[0]= 0;
	  switch (c)
	    {
	    case 'A': return TU_UP;
	    case 'B': return TU_DOWN;
	    case 'C': return TU_RIGHT;
	    case 'D': return TU_LEFT;
	    case 'H': return TU_HOME;
	    case 'F': return TU_END;
	    case 'E': tu_esc_buffer[0]= 0; return 0; // NumPad 5
	    }
	}
      else if (c == '~')
	{
	  int n;
	  tu_esc_buffer[0]= 0;
	  n= strtol(&tu_esc_buffer[2], 0, 0);
	  switch (n)
	    {
	    case 1: return TU_HOME;
	    case 2: return TU_INS;
	    case 3: return TU_DEL;
	    case 4: return TU_END;
	    case 5: return TU_PGUP;
	    case 6: return TU_PGDOWN;
	    case 11: return TU_F1;
	    case 12: return TU_F2;
	    case 13: return TU_F3;
	    case 14: return TU_F4;
	    case 15: return TU_F5;
	    case 17: return TU_F6;
	    case 18: return TU_F7;
	    case 19: return TU_F8;
	    case 20: return TU_F9;
	    case 21: return TU_F10;
	    case 23: return TU_F11;
	    case 24: return TU_F12;
	    default: tu_esc_buffer[0]= 0; return 0;
	    }
	}
      else
	return 0;
    }
  else
    {
      if (c == '\033')
	{
	  tu_esc_buffer[0]= '\033', tu_esc_buffer[1]= 0;
	  return 0;
	}
    }
  return c;
}

static char tu_line[tu_buf_size]= { 0 };
static int tu_cursor= 0;
static int tu_ready= 0;

static void
tu_process(char c)
{
  int k, i;
  if (tu_raw_mode)
    {
      // RAW
      if (do_process_esc)
	{
	  k= tu_proc_esc(c);
	  if (k)
	    tu_put(k);
	  return;
	}
      tu_put(c);
    }
  else
    {
      // COOCKED
      int l= strlen(tu_line);
      k= tu_proc_esc(c);
      if (!k || tu_ready)
	return;
      // CURSOR MOVEMENT
      if (k == TU_LEFT)
	{
	  if (tu_cursor > 0)
	    {
	      tu_cursor--;
	      tu_go_left(1);
	    }
	}
      else if (k == TU_RIGHT)
	{
	  if (tu_line[tu_cursor] != 0)
	    {
	      tu_cursor++;
	      tu_go_right(1);
	    }
	}
      else if ((k == TU_HOME) ||
	       (k == 'A'-'A'+1))
	{
	  if (tu_cursor > 0)
	    {
	      tu_go_left(tu_cursor);
	      tu_cursor= 0;
	    }
	}
      else if ((k == TU_END) ||
	       (k == 'E'-'A'+1))
	{
	  if (tu_line[tu_cursor] != 0)
	    {
	      tu_go_right(l-tu_cursor);
	      tu_cursor= l;
	    }
	}
      // FINISH EDITING
      else if ((k == 'C'-'A'+1) ||
	       (k == 'D'-'A'+1))
	{
	  tu_ready= 1;
	}
      else if ((k == '\n') ||
	       (k == '\r'))
	{
	  tu_ready= 1;
	  for (i= 0; i<l; i++)
	    tu_put(tu_line[i]);
	  //tu_cooked();
	}
      // DELETING
      else if (k == 8 /*BS*/)
	{
	  if (tu_cursor > 0)
	    {
	      for (i= tu_cursor; tu_line[i]; i++)
		tu_line[i-1]= tu_line[i];
	      l--;
	      tu_line[l]= 0;
	      tu_cursor--;
	      tu_go_left(1);
	      tu_save_cursor();
	      if (tu_line[tu_cursor])
		printf("%s ", &tu_line[tu_cursor]), fflush(stdout);
	      else
		write(STDOUT_FILENO, " ", 1);
	      tu_restore_cursor();
	    }
	}
      else if ((k == 127) || /*DEL*/
	       (k == TU_DEL))
	{
	  if (tu_line[tu_cursor] != 0)
	    {
	      for (i= tu_cursor+1; tu_line[i]; i++)
		tu_line[i-1]= tu_line[i];
	      l--;
	      tu_line[l]= 0;
	      tu_save_cursor();
	      if (tu_line[tu_cursor])
		printf("%s ", &tu_line[tu_cursor]), fflush(stdout);
	      else
		write(STDOUT_FILENO, " ", 1);
	      tu_restore_cursor();
	    }
	}
      else if (k == 'K'-'A'+1)
	{
	  if (tu_cursor > 0)
	    tu_go_left(tu_cursor);
	  tu_save_cursor();
	  while (l--)
	    write(STDOUT_FILENO, " ", 1);
	  tu_restore_cursor();
	  tu_line[tu_cursor= 0]= 0;
	}
      else if (k < 0)
	;
      else if (isprint(k))
	{
	  if (l < tu_buf_size)
	    {
	      if (tu_line[tu_cursor] == 0)
		{
		  tu_line[tu_cursor++]= k;
		  tu_line[tu_cursor]= 0;
		  write(1, &c, 1);
		}
	      else
		{
		  int j;
		  for (j= l; j>=tu_cursor; j--)
		    tu_line[j+1]= tu_line[j];
		  tu_line[tu_cursor++]= k;
		  tu_save_cursor();
		  write(1, &tu_line[tu_cursor-1], l-tu_cursor+2);
		  tu_restore_cursor();
		  tu_go_right(1);
		}
	    }
	}
    }
}

extern int ols_input_avail(int fd);

int
tu_input_avail()
{
  while (//uart_avail()
		  ols_input_avail(fileno(stdin))
		  )
    {
      char c;
      int i= read(fileno(stdin), &c, 1);
      if (i>=0)
	tu_process(c);
    }
  if (tu_raw_mode)
    return tu_last_used != tu_first_free;
  return tu_ready;
}

int
tu_getch()
{
  tu_input_avail();
  if (!tu_raw_mode)
    return 0;
  return tu_get();
}

int
tu_getc()
{
	tu_raw_mode= 1;
	tu_input_avail();
	return tu_get();
}

int
tu_gets(char *buf, int len)
{
  int k, i= 0;
  tu_raw_mode= 0;
  tu_input_avail();
  if (!buf)
    return 0;
  k= tu_get();
  buf[0]= 0;
  while ((k != 0) &&
	 (i < len-1))
    {
      if (k > 0)
	{
	  buf[i]= k;
	  i++;
	  buf[i]= 0;
	}
      k= tu_get();
    }
  if ((k= (i>0) || tu_ready) != 0)
    tu_cooked();
  return k;
}

void
tu_cooked()
{
  tu_raw_mode= 0;
  tu_line[tu_cursor= 0]= 0;
  tu_ready= 0;
}

void
tu_raw()
{
  tu_raw_mode= 1;
}


/******************************************************************** OUTPUT */

void
tu_save_cursor()
{
  printf("\033[s");
  fflush(stdout);
}

void
tu_restore_cursor()
{
  printf("\033[u");
  fflush(stdout);
}

void
tu_go_left(int n)
{
  printf("\033[%dD", n);
  fflush(stdout);
}

void
tu_go_right(int n)
{
  printf("\033[%dC", n);
  fflush(stdout);
}

void
tu_go_up(int n)
{
  printf("\033[%dA", n);
  fflush(stdout);
}

void
tu_go_down(int n)
{
  printf("\033[%dB", n);
  fflush(stdout);
}

void
tu_go(int x1, int y1)
{
  printf("\033[%d;%dH", y1, x1);
  fflush(stdout);
}

void
tu_clear_screen()
{
  printf("\033[2J");
  tu_go(1, 1);
  fflush(stdout);
}

void
tu_clear_char()
{
  tu_save_cursor();
  printf(" ");
  tu_restore_cursor();
}

static int tu_bg_color, tu_fg_color;

void
tu_color(int bg, int fg)
{
  if (bg >= 0)
    printf("\033[%dm", (tu_bg_color= bg)+40);
  if (fg >= 0)
    printf("\033[%dm", (tu_fg_color= fg)+30);
  fflush(stdout);
}

void
tu_hide(void)
{
  printf("\033[?25l");
  fflush(stdout);
}

void
tu_show(void)
{
  printf("\033[?25h");
  fflush(stdout);
}

void
tu_mouse_on(void)
{
  // enable mouse click reports of terminal
  printf("\033[1;2'z"); // enable locator reporting, as character cells
  printf("\033[?9h"); // send mouse X,Y on btn press (X10 mode)
  fflush(stdout);
}

void
tu_mouse_off(void)
{
  printf("\033[?9l"); // do not send mouse X,Y on btn press
  printf("\033[0;0'z"); // disable locator reporting
  fflush(stdout);
}

/* End of terminal.c */
