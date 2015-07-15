#include "clock.h"
#include "3310_routines.h"
#include "misc.h"
#include "bool.h"
#include "hardware.h"


uint32_t timer=0;
bool timer_counting=false;

void clock_start()
{
	timer_counting=true;
	//reset_oneps_loop();
}

void clock_stop()
{
	timer_counting=false;
	LCD_writeString_XY(CLOCK_NOTIF_X,0," ",OVERWRITE);
}

void clock_toggle()
{
	if(timer_counting)
		clock_stop();
	else
		clock_start();
}

void clock_count()
{
	if(timer_counting)
	{
		timer++;
		if(timer%CLOCK_PRESCALE < (CLOCK_PRESCALE/2) )
			LCD_writeString_XY(CLOCK_NOTIF_X,0,"W",OVERWRITE);
		else
			LCD_writeString_XY(CLOCK_NOTIF_X,0," ",OVERWRITE);
	}
}

void clock_reset()
{
	clock_stop();
	timer=0;
	clock_draw();
}

void clock_draw_cond()
{
	if(timer%(CLOCK_PRESCALE)==0 && timer_counting)
		clock_draw();
}

void clock_draw()
{
	uint16_t prescaled=timer/CLOCK_PRESCALE;

	LCD_writeString_XY(20,2,itoa(prescaled/60,3),OVERWRITE);
	LCD_writeChar(':',OVERWRITE);
	LCD_writeString_XY(0,0,itoa_fill(prescaled%60,2,'0'),OVERWRITE|RELATIVE);
	if(prescaled%60==0 && timer!=0)
		beep(BEEP_CLOCK_MINUTE);
}
