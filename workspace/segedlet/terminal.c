#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "terminal.h"
//#include "vcp.h"
#include "ols_fs.h"

struct tu_input_t ti_std= {
		.ifd= 0,
		.ofd= 1,
		.process_esc= 1
};

static void
ti_put(struct tu_input_t *ti, int c)
{
  int new= (ti->first_free+1)%tu_buf_size;
  if (new == ti->last_used)
    return;
  ti->buffer[ti->first_free]= c;
  ti->first_free= new;
}

static int
ti_get(struct tu_input_t *ti)
{
  int c= ti->buffer[ti->last_used];
  if (ti->first_free == ti->last_used)
    return 0;
  ti->last_used= (ti->last_used+1)%tu_buf_size;
  return c;
}

static int
ti_proc_esc(struct tu_input_t *ti, char c)
{
  if (ti->esc_buffer[0] == '\033')
    {
      int l= strlen(ti->esc_buffer);
      ti->esc_buffer[l]= c;
      l++;
      ti->esc_buffer[l]= 0;
      if ((l > 2) &&
	  ti->esc_buffer[2] == 'M')
	{
	  // Mouse
	  if (l == 6)
	    {
	      int /*f,*/ x, y, ret= 0;
	      switch (ti->esc_buffer[3])
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
	      x= (ti->esc_buffer[4] - 0x20) & 0xff;
	      y= (ti->esc_buffer[5] - 0x20) & 0xff;
	      ret|= x << 16;
	      ret|= y << 8;
	      ti->esc_buffer[0]= 0;
	      return ret;
	    }
	}
      if (ti->esc_buffer[1] != '[')
	{
	  ti->esc_buffer[0]= 0;
	  return c;
	}
      if (isalpha((int)c))
	{
	  ti->esc_buffer[0]= 0;
	  switch (c)
	    {
	    case 'A': return TU_UP;
	    case 'B': return TU_DOWN;
	    case 'C': return TU_RIGHT;
	    case 'D': return TU_LEFT;
	    case 'H': return TU_HOME;
	    case 'F': return TU_END;
	    case 'E': ti->esc_buffer[0]= 0; return 0; // NumPad 5
	    }
	}
      else if (c == '~')
	{
	  int n;
	  ti->esc_buffer[0]= 0;
	  n= strtol(&(ti->esc_buffer[2]), 0, 0);
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
	    default: ti->esc_buffer[0]= 0; return 0;
	    }
	}
      else
	return 0;
    }
  else
    {
      if (c == '\033')
	{
	  ti->esc_buffer[0]= '\033', ti->esc_buffer[1]= 0;
	  return 0;
	}
    }
  return c;
}

static void
ti_process(struct tu_input_t *ti, char c)
{
  int k, i;
  if (ti->raw_mode)
    {
      // RAW
      if (ti->process_esc)
	{
	  k= ti_proc_esc(ti, c);
	  if (k)
	    ti_put(ti, k);
	  return;
	}
      ti_put(ti, c);
    }
  else
    {
      // COOCKED
      int l= strlen(ti->line);
      k= ti_proc_esc(ti, c);
      if (!k || ti->ready)
	return;
      // CURSOR MOVEMENT
      if (k == TU_LEFT)
	{
	  if (ti->cursor > 0)
	    {
	      ti->cursor--;
	      dtu_go_left(ti->ofd, 1);
	    }
	}
      else if (k == TU_RIGHT)
	{
	  if (ti->line[ti->cursor] != 0)
	    {
	      (ti->cursor)++;
	      dtu_go_right(ti->ofd, 1);
	    }
	}
      else if ((k == TU_HOME) ||
	       (k == 'A'-'A'+1))
	{
	  if (ti->cursor > 0)
	    {
	      dtu_go_left(ti->ofd, ti->cursor);
	      ti->cursor= 0;
	    }
	}
      else if ((k == TU_END) ||
	       (k == 'E'-'A'+1))
	{
	  if (ti->line[ti->cursor] != 0)
	    {
	      dtu_go_right(ti->ifd, l-ti->cursor);
	      ti->cursor= l;
	    }
	}
      // FINISH EDITING
      else if ((k == 'C'-'A'+1) ||
	       (k == 'D'-'A'+1))
	{
	  ti->ready= 1;
	}
      else if ((k == '\n') ||
	       (k == '\r'))
	{
	  ti->ready= 1;
	  for (i= 0; i<l; i++)
	    ti_put(ti, ti->line[i]);
	  //tu_cooked();
	}
      // DELETING
      else if (k == 8 /*BS*/)
	{
	  if (ti->cursor > 0)
	    {
	      for (i= ti->cursor; ti->line[i]; i++)
	    	  ti->line[i-1]= ti->line[i];
	      l--;
	      ti->line[l]= 0;
	      ti->cursor--;
	      dtu_go_left(ti->ofd, 1);
	      dtu_save_cursor(ti->ofd);
	      if (ti->line[ti->cursor])
	      {
	    	  dprintf(ti->ofd, "%s ", &(ti->line[ti->cursor]));
	    	  fflush(NULL);
	      }
	      else
	    	  write(ti->ofd, " ", 1);
	      dtu_restore_cursor(ti->ofd);
	    }
	}
      else if ((k == 127) || /*DEL*/
	       (k == TU_DEL))
	{
	  if (ti->line[ti->cursor] != 0)
	    {
	      for (i= ti->cursor+1; ti->line[i]; i++)
		ti->line[i-1]= ti->line[i];
	      l--;
	      ti->line[l]= 0;
	      dtu_save_cursor(ti->ofd);
	      if (ti->line[ti->cursor])
	      {
	    	  dprintf(ti->ofd, "%s ", &(ti->line[ti->cursor]));
	    	  fflush(NULL);
	      }
	      else
	    	  write(ti->ofd, " ", 1);
	      dtu_restore_cursor(ti->ofd);
	    }
	}
      else if (k == 'K'-'A'+1)
	{
	  if (ti->cursor > 0)
	    dtu_go_left(ti->ofd, ti->cursor);
	  dtu_save_cursor(ti->ofd);
	  while (l--)
	    write(ti->ofd, " ", 1);
	  dtu_restore_cursor(ti->ofd);
	  ti->line[ti->cursor= 0]= 0;
	}
      else if (k < 0)
	;
      else if (isprint(k))
	{
	  if (l < tu_buf_size)
	    {
	      if (ti->line[ti->cursor] == 0)
		{
		  ti->line[(ti->cursor)++]= k;
		  ti->line[ti->cursor]= 0;
		  write(ti->ofd, &c, 1);
		}
	      else
		{
		  int j;
		  for (j= l; j>=ti->cursor; j--)
		    ti->line[j+1]= ti->line[j];
		  ti->line[(ti->cursor)++]= k;
		  dtu_save_cursor(ti->ofd);
		  write(ti->ofd, &(ti->line[ti->cursor-1]), l-ti->cursor+2);
		  dtu_restore_cursor(ti->ofd);
		  dtu_go_right(ti->ofd, 1);
		}
	    }
	}
    }
}

int
ti_input_avail(struct tu_input_t *ti)
{
  while (ols_input_avail(ti->ifd))
    {
      char c;
      int i= read(ti->ifd, &c, 1);
      if (i>=0)
	ti_process(ti, c);
    }
  if (ti->raw_mode)
    return ti->last_used != ti->first_free;
  return ti->ready;
}

int
ti_getch(struct tu_input_t *ti)
{
  ti_input_avail(ti);
  if (!ti->raw_mode)
    return 0;
  return ti_get(ti);
}

int tu_getch()
{
	return ti_getch(&ti_std);
}

int
ti_getc(struct tu_input_t *ti)
{
	ti->raw_mode= 1;
	ti_input_avail(ti);
	return ti_get(ti);
}

int tu_getc()
{
	return ti_getc(&ti_std);
}

int
ti_gets(struct tu_input_t *ti, char *buf, int len)
{
  int k, i= 0;
  ti->raw_mode= 0;
  ti_input_avail(ti);
  if (!buf)
    return 0;
  k= ti_get(ti);
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
      k= ti_get(ti);
    }
  if ((k= (i>0) || ti->ready) != 0)
    ti_cooked(ti);
  return k;
}

int tu_gets(char *buf, int len)
{
	return ti_gets(&ti_std, buf, len);
}

void
ti_cooked(struct tu_input_t *ti)
{
  ti->raw_mode= 0;
  ti->line[ti->cursor= 0]= 0;
  ti->ready= 0;
}

void tu_cooked()
{
	ti_cooked(&ti_std);
}

void
ti_raw(struct tu_input_t *ti)
{
  ti->raw_mode= 1;
  ti->esc_buffer[0]= 0;
}

void tu_raw()
{
	ti_raw(&ti_std);
}

void
ti_process_esc(struct tu_input_t *ti, int proc)
{
	ti->process_esc= proc;
}

void tu_process_esc(int proc)
{
	ti_process_esc(&ti_std, proc);
}


/******************************************************************** OUTPUT */

void
dtu_save_cursor(int fd)
{
	dprintf(fd, "\033[s");
	fflush(NULL);
}

void
ftu_save_cursor(FILE *f)
{
	dtu_save_cursor(fileno(f));
}

void
tu_save_cursor()
{
	dtu_save_cursor(STDOUT_FILENO);
}


/*--------------------------------*/

void
dtu_restore_cursor(int fd)
{
	dprintf(fd, "\033[u");
	fflush(NULL);
}

void
ftu_restore_cursor(FILE *f)
{
	dtu_restore_cursor(fileno(f));
}

void
tu_restore_cursor()
{
	dtu_restore_cursor(STDOUT_FILENO);
}


/*--------------------------------*/

void
dtu_go_left(int fd, int n)
{
  dprintf(fd, "\033[%dD", n);
  fflush(NULL);
}

void
ftu_go_left(FILE *f, int n)
{
	dtu_go_left(fileno(f), n);
}

void
tu_go_left(int n)
{
	dtu_go_left(STDOUT_FILENO, n);
}


/*--------------------------------*/

void
dtu_go_right(int fd, int n)
{
	dprintf(fd, "\033[%dC", n);
	fflush(NULL);
}

void
ftu_go_right(FILE *f, int n)
{
	dtu_go_right(fileno(f), n);
}

void
tu_go_right(int n)
{
	dtu_go_right(STDOUT_FILENO, n);
}


/*--------------------------------*/

void
dtu_go_up(int fd, int n)
{
	dprintf(fd, "\033[%dA", n);
	fflush(NULL);
}

void
ftu_go_up(FILE *f, int n)
{
	dtu_go_up(fileno(f), n);
}

void
tu_go_up(int n)
{
	dtu_go_up(STDOUT_FILENO, n);
}


/*--------------------------------*/

void
dtu_go_down(int fd, int n)
{
	dprintf(fd, "\033[%dB", n);
	fflush(NULL);
}

void
ftu_go_down(FILE *f, int n)
{
	dtu_go_down(fileno(f), n);
}

void
tu_go_down(int n)
{
	dtu_go_down(STDOUT_FILENO, n);
}


/*--------------------------------*/

void
dtu_go(int fd, int x1, int y1)
{
	dprintf(fd, "\033[%d;%dH", y1, x1);
	fflush(NULL);
}

void
ftu_go(FILE *f, int x1, int y1)
{
	dtu_go(fileno(f), x1, y1);
}

void
tu_go(int x1, int y1)
{
	dtu_go(STDOUT_FILENO, x1, y1);
}


/*--------------------------------*/

void
dtu_clear_screen(int fd)
{
	dprintf(fd, "\033[2J");
	dtu_go(fd, 1, 1);
	fflush(NULL);
}

void
ftu_clear_screen(FILE *f)
{
	dtu_clear_screen(fileno(f));
}

void
tu_clear_screen()
{
	dtu_clear_screen(STDOUT_FILENO);
}


/*--------------------------------*/

void
dtu_clear_char(int fd)
{
	dtu_save_cursor(fd);
	dprintf(fd, " ");
	dtu_restore_cursor(fd);
}

void
ftu_clear_char(FILE *f)
{
	dtu_clear_char(fileno(f));
}

void
tu_clear_char()
{
	dtu_clear_char(STDOUT_FILENO);
}


/*--------------------------------*/

//static int tu_bg_color, tu_fg_color;

/*
        FG   BG
Normal  30   40
Bright  90  100
 */
void
dtu_color(int fd, enum tu_color_t bg, enum tu_color_t fg)
{
  if ((int)bg >= 0)
    bg+= (bg&0x10)?100-0x10:40;
  if ((int)fg >= 0)
    fg+= (fg&0x10)?90-0x10:30;
  if (((int)bg >= 0) && ((int)fg >= 0))
    dprintf(fd, "\033[%d;%dm", bg, fg);
  else if ((int)bg >= 0)
    dprintf(fd, "\033[%dm", bg);
  else if ((int)fg >= 0)
    dprintf(fd, "\033[%dm", fg);
  fflush(NULL);
}

void
ftu_color(FILE *f, enum tu_color_t bg, enum tu_color_t fg)
{
  dtu_color(fileno(f), bg, fg);
}

void
tu_color(enum tu_color_t bg, enum tu_color_t fg)
{
  dtu_color(STDOUT_FILENO, bg, fg);
}


/*--------------------------------*/

void
dtu_hide(int fd)
{
	dprintf(fd, "\033[?25l");
	fflush(NULL);
}

void
ftu_hide(FILE *f)
{
	dtu_hide(fileno(f));
}

void
tu_hide()
{
	dtu_hide(STDOUT_FILENO);
}


/*--------------------------------*/

void
dtu_show(int fd)
{
	dprintf(fd, "\033[?25h");
	fflush(NULL);
}

void
ftu_show(FILE *f)
{
	dtu_show(fileno(f));
}

void
tu_show(void)
{
	dtu_show(STDOUT_FILENO);
}


/*--------------------------------*/

void
dtu_mouse_on(int fd)
{
	// enable mouse click reports of terminal
	dprintf(fd, "\033[1;2'z"); // enable locator reporting, as character cells
	dprintf(fd, "\033[?9h"); // send mouse X,Y on btn press (X10 mode)
	fflush(NULL);
}

void
ftu_mouse_on(FILE *f)
{
	dtu_mouse_on(fileno(f));
}

void
tu_mouse_on()
{
	dtu_mouse_on(STDOUT_FILENO);
}


/*--------------------------------*/

void
dtu_mouse_off(int fd)
{
	dprintf(fd, "\033[?9l"); // do not send mouse X,Y on btn press
	dprintf(fd, "\033[0;0'z"); // disable locator reporting
	fflush(NULL);
}

void
ftu_mouse_off(FILE *f)
{
	dtu_mouse_off(fileno(f));
}

void
tu_mouse_off()
{
	dtu_mouse_off(STDOUT_FILENO);
}


/* End of terminal.c */
