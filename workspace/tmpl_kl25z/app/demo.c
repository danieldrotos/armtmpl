#include <stdio.h>
#include <unistd.h>
#include <ctype.h>

#include "segedlet.h"
#include "terminal.h"

void
ts_show_bar()
{
	static int last_shown;
	int i;

	if (tsi_value == last_shown)
		return;
	if (!tsi_touch())
		return;
	tu_hide();
	tu_save_cursor();
	tu_go(1,2);
	tu_color(0,7);
	printf("%3d ", tsi_value);
	for (i=0; i<= tsi_value && i<75; i++)
		printf("=");
	for ( ; i<75; i++)
		printf(" ");
	tu_restore_cursor();
	tu_show();
	last_shown= tsi_value;
}

void ts_print_bar()
{
	//static int prev_tsi;
	int i, v= tsi_value;
	if (tsi_touch() /*&& (prev_tsi!=v)*/)
	{
		tu_color(0,7);
		printf("%3d ", v);
		for (i=0;i<75;i++)
			printf("%c", (i<=v)?'=':' ');
		printf("\n");
		//prev_tsi= v;
	}
}

int steps, fe= 50;

int
blue_proc(int c)
{
    volatile int cnt= xHWREG(PWMA_BASE+TPM_CNT);
    unsigned int cv;
    int duty;
    if ((isprint((int)c)||(c=='\n')||(c=='\r')))
  	  printf("%c", c);
    else
  	  printf(" _%d_", c);
    if (c == 's')
      {
        printf("steps= %d\n", steps);
        printf("clock src= %08lx\n", xHWREG(0x40048004));
      }
    if (c=='v')
      printf("cnt= %d\n", cnt);
    if (c=='+')
      {
        if (fe<100)
        	fe+= 10;
        else
        	fe= 100;
        duty= light2duty(fe);
        steps= blue_pwm(1000, duty);
        cv= xHWREG(PWMA_BASE+TPM_C1V);
        printf("fe=%d%% steps= %d/cv= %u (duty=%d%%)\n", fe, steps, cv, duty);
      }
    if (c=='-')
      {
    	if (fe>0)
    		fe-= 10;
    	else
    		fe= 0;
        duty= light2duty(fe);
        steps= blue_pwm(1000, duty);
        cv= xHWREG(PWMA_BASE+TPM_C1V);
        printf("fe= %d%% steps= %d/cv= %u (duty=%d%%)\n", fe, steps, cv, duty);
      }
    else
    	return c;
    fflush(stdout);
    return 0;
}

void
blue_demo()
{
	int c;
    if ((c= tu_getc()))
      {
        blue_proc(c);
      }
}

