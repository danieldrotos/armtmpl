#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "temu.h"


static int
get_n(int params[], int valids[], int i, int def)
{
  if (i>=10)
    return def;
  if (params[i] < 0)
    return def;
  if (valids[i])
    return params[i];
  return def;
}

enum temu_ret_t temu_print(struct temu_t *te, char c)
{
  if (te->esc_state == 1)
    {
      te->esc_state= 0;
      switch (c)
      {
        case 'N': // SS2 next char G2 font
          break;
        case 'O': // SS3 next char G3 font
          break;
        case 'P': // DCS device control string
          break;
        case '^': // PM string, terminated by ESC\ ;
          break;
        case '_': // APC string, terminated by ESC\ ;
          break;
        case ']': // OSC operating system command
          break;
        case '[': // start CSI
          {
            int j;
            te->esc_state= 2;
            te->esc_seq[te->esc_idx= 0]= '\0';
            for (j= 0; j<esc_size; j++)
              te->esc_seq[j]= 0;
            break;
          }
      }
    }
  else if (te->esc_state == 2)
    {
      if ((c >= '@') && (c <= '~'))
        {
          int p;
          char *t, *e;
          int n, m;
          // sequence finished, interpret
          if (strcmp(te->esc_seq, "?25") == 0)
            {
              te->esc_state= 0;
              if (c=='h')
                return temu_show_cursor;
              if (c=='l')
                return temu_hide_cursor;
              return 0;
            }
          for (p= 0; p<10; p++)
            te->valid[p]= 0, te->params[p]= -1;
          p= 0;
          t= te->esc_seq;
          for (e= t; *e; e++) ;
          if (*t && (*t == ';'))
            {
              te->params[p]= 0;
              te->valid[p]= 0;
              p++;
              t++;
            }
          while (*t &&
              (t < e) &&
              (isdigit((int)(*t)) || (*t == ';')) &&
              (p < 10))
            {
              char *tail;
              if (*t == ';')
                {
                  te->params[p]= 0;
                  te->valid[p]= 0;
                  t++;
                }
              else
                {
                  te->params[p]= strtol(t, &tail, 10);
                  te->valid[p]= (tail == t)?0:1;
                  t= tail+1;
                }
              p++;
            }
          n= get_n(te->params, te->valid, 0, 1);
          m= get_n(te->params, te->valid, 1, 1);
          te->esc_state= 0;
          switch (c)
          {
            case 'A': // n CUU curs up by n/1
              te->curs_y-= n;
              if (te->curs_y < 0)
                te->curs_y= 0;
              break;
            case 'B': // n CUD move cursor down by n/1 cells
              te->curs_y+= n;
              if (te->curs_y >= te->rows)
                te->curs_y= te->rows-1;
              break;
            case 'C': // n CUF move cursor forward by n/1 cells
              te->curs_x+= n;
              if (te->curs_x >= te->cols)
                te->curs_x= te->cols-1;
              break;
            case 'D': // n CUB move cursor backward by n/1 cells
              te->curs_x-= n;
              if (te->curs_x < 0)
                te->curs_x= 0;
              break;
            case 'E': // n CNL move cursor to begining of line n/1 lines down
              te->curs_y+= n;
              te->curs_x= 0;
              if (te->curs_y >= te->rows)
                te->curs_y= te->rows-1;
              break;
            case 'F': // n CPL move cursor to beginning of line n/1 lines up
              te->curs_y-= n;
              te->curs_x= 0;
              if (te->curs_y < 0)
                te->curs_y= 0;
              break;
            case 'G': // n CHA move cursor to column n
              te->curs_x= n-1;
              if (te->curs_x < 0)
                te->curs_x= 0;
              break;
            case 'H': // n;m CUP move cursor to row n/1, column m/1
            case 'f': // n;m HVP move cursor to tow n/1, column m/1
              te->curs_x= m-1;
              te->curs_y= n-1;
              if (te->curs_x < 0)
                te->curs_x= 0;
              if (te->curs_y < 0)
                te->curs_y= 0;
              if (te->curs_y >= te->rows)
                te->curs_y= te->rows-1;
              if (te->curs_x >= te->cols)
                te->curs_x= te->cols-1;
              break;
            case 'J': // n ED n=0: clear below cursor,
              //      n=1: clear above cursor,
              //      n=2: clear entire screen, move cursor home
              n= get_n(te->params, te->valid, 0, 0);
              if (n == 0)
                return temu_clear_below_cursor;
              if (n == 1)
                return temu_clear_above_cursor;
              if (n == 2)
                {
                  te->curs_x= 0;
                  te->curs_y= 0;
                  return temu_clear_entire_screen;
                }
              break;
            case 'K': 	// n EL n=0: clear line left from cursor
              //      n=1: clear line right from cursor
              //      n=2: clear enire line
              n= get_n(te->params, te->valid, 0, 0);
              if (n == 0)
                return temu_clear_left;
              if (n == 1)
                return temu_clear_right;
              if (n == 2)
                return temu_clear_line;
              break;
            case 'S': // n SU scroll up by n/1 lines
              break;
            case 'T': // n SD scroll down by n/1 lines
              break;
            case 'm': // n;... set SGR parameter(s)
              for (p= 0; p < 10; p++)
                {
                  if (te->valid[p])
                    {
                      if (te->params[p] == 0)
                        {
                          te->sgr_font= 0;
                          te->sgr_flags= sgr_none;
                          te->sgr_fg= 7;
                          te->sgr_bg= 0;
                        }
                      else if (te->params[p] < 10)
                        {
                          te->sgr_flags|= 1 << (te->params[p]-1);
                        }
                      else if (te->params[p] == 10)
                        {
                          te->sgr_font= 0;
                        }
                      else if (te->params[p] < 20)
                        {
                          n= te->params[p] - 10;
                          if (n >= te->nuof_fonts)
                            n= 0;
                          te->sgr_font= n;
                        }
                      else if (te->params[p] == 20)
                        {
                          // fraktur?
                        }
                      else if (te->params[p] < 30)
                        {
                          n= te->params[p] - 20;
                        }
                      else if (te->params[p] < 38)
                        {
                          // Set FG color
                          n= te->params[p] - 30;
                        }
                      else if (te->params[p] == 38)
                        {
                          // extended fg color
                        }
                      else if (te->params[p] == 39)
                        {
                          te->sgr_fg= 7;
                        }
                      else if (te->params[p] < 48)
                        {
                          // Set BG color
                          n= te->params[p] - 40;
                        }
                      else if (te->params[p] == 48)
                        {
                          // extended bg color
                        }
                      else if (te->params[p] == 49)
                        {
                          te->sgr_bg= 0;
                        }
                      else
                        {
                          // not supported...
                        }
                    }
                }
              return temu_set_sgr;
              break;
            case 'i': // 5|4 AUX En/Dis aux serial port
              break;
            case 'n': // 6 DSR report cursor position
              break;
            case 's': // SCP save cursor pos
              te->saved_x= te->curs_x;
              te->saved_y= te->curs_y;
              break;
            case 'u': // RCP restore cursor pos
              te->curs_x= te->saved_x;
              te->curs_y= te->saved_y;
              break;
          }
        }
      else if (te->esc_idx >= esc_size)
        {
          // too long, drop
          te->esc_state= 0;
        }
      else
        {
          te->esc_seq[(te->esc_idx)++]= c;
          te->esc_seq[te->esc_idx]= '\0';
        }
    }
  else if (c == 0x1b)
    {
      // ESC
      te->esc_state= 1;
      te->esc_seq[te->esc_idx= 0]= 0;
    }
  else if (c == 155)
    {
      // Single char CSI
      te->esc_state= 2;
      te->esc_seq[te->esc_idx= 0]= 0;
    }
  else if (c == '\n')
    {
      te->curs_x= 0;
      te->curs_y++;
    }
  else if (c == '\r')
    {
      te->curs_x= 0;
    }
  else
    return c;
  return 0;
}
