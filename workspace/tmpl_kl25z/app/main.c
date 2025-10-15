#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>

#include "xhw_types.h"
#include "xhw_memmap.h"
#include "xcore.h"
#include "xsysctl.h"
#include "xgpio.h"
#include "xwdt.h"
#include "xuart.h"

#include "segedlet.h"
#include "terminal.h"
#include "demo.h"
#include "shoot_game.h"

unsigned int blink_time= 500;

void blink()
{
	static unsigned int last;
	if (clk-last > blink_time)
    {
		last= clk;
		red_toggle();
    }
}

int light, period= 800, shining= 1;

void shine()
{
	static unsigned int last;
	static int up;
	float steptime= period/100.0;
	if (!shining)
	{
		blue_off();
		return;
	}
	if (clk-last > steptime)
	{
		last= clk;
		if (up)
		{
			if (++light > 100)
			{
				light= 100;
				up= 0;
			}
		}
		else
		{
			if (--light < 0)
			{
				light= 0;
				up= 1;
			}
		}
		blue_light(light);
	}
}
int main(void)
{
	board_init();
	blue_light(light= 50);
	tu_color(0,7);
	tu_clear_screen();
	printf("Hello World!\n");
	while(1)
    {
		//blink();
		shine();
		shoot_game();
		ts_show_bar();
		//ts_print_bar();
		tsi_loop();
		if (tsi_touch())
		{
			shining= 0;
			blink_time= 10+tsi_value*8;
			blink();
        }
		else
        {
			shining= 1;
			period= blink_time= 800;
			red_off();
        }

    }
}
