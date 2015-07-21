#include "screen.h"
#include "3310_routines.h"
#include "misc.h"
#include "analog.h"

uint8_t menu_state;
void (*screen_actual_draw_function)(void)=NULL;
volatile screen_t screen={0,0};

void screen_schedule_redraw(uint8_t delay_ds)
{
	screen.redraw=delay_ds+1;//because redraw occurs on 1, 0 means idle
}

void screen_redraw()
{
	LCD_clear();
	screen_template_draw();
	screen_battery_draw(0);
	if(screen_actual_draw_function!=NULL)
		screen_actual_draw_function();
	screen.redraw=0;
}

void screen_battery_draw(uint8_t addflag)
{
	LCD_writeString_XY(59,0,itoa_dec(adc_battery_voltage,3,1),OVERWRITE|addflag);
}

void screen_template_draw()
{
	LCD_drawHline(0,10,HW_COLUMNS);
}
