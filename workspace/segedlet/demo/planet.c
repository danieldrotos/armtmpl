#include <stdio.h>

#include "terminal.h"

#include "planet.h"


float hmax= 20.1;
int max_row= 23;

struct planet_t planets[]=
    {
        { .g=279.6 , .col=0, .title= "SUN" }, // sun
        { .g=  3.7 , .col=0, .title= "MER" }, // mercury
        { .g=  8.9 , .col=0, .title= "VEN" }, // venus
        { .g=  9.81, .col=0, .title= "EAR" }, // earth
        { .g=  3.7 , .col=0, .title= "MAR" }, // mars
        { .g= 23.1 , .col=0, .title= "JUP" }, // jupiter
        { .g=  9.0 , .col=0, .title= "SAT" }, // saturn
        { .g=  8.7 , .col=0, .title= "URA" }, // uranus
        { .g= 11.0 , .col=0, .title= "NEP" }, // neptun
        { .g=  0.7 , .col=0, .title= "PLU" }, // pluto
        { .col=-1 }
    };


void show(struct planet_t *p)
{
  if (p->last_row != p->row)
    {
      tu_go(p->col, p->last_row); printf(" ");
      tu_go(p->col, p->row);      printf("@");
      p->last_row= p->row;
    }
}
