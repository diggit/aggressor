#include "homescreen.h"
#include "3310_routines.h"
#include "screen.h"
#include "menu.h"
#include "control.h"
#include "clock.h"
#include "menu_model.h"
#include "input.h"
#include "systick.h"
#include "telemetry.h"

#include "hardware.h"

//homescreen setters, if they returns zero, homescreen is not available
//eg. telemetry may not be available all the times
//if non zero returned, homescreen was set
//setter itself has to set periodical calls etc.

//index of active homescreen in homescreens array
uint8_t current_homescreen_num=0;//do not change


//array of homescreen setters, they are called on change of homescreen
uint8_t (*homescreens[])(void )={\
	homescreen_input_bars_set,\
	homescreen_telemetry_set};

uint8_t telemetry_timeout=0;

//here starts setters and custom homescreen drawers itself----------------------------
uint8_t homescreen_telemetry_set()
{
	if (telemetry.enabled)//check if telemetry is available
	{
		if(telemetry.updated!=1)//if there are no data ready, set timeout, NO TELEMETRY message will be shown immediately
			telemetry_timeout=TELEMETRY_TIMEOUT;
		homescreen_telemetry_draw_template();
		fiftyps_periodical_call=homescreen_telemetry_draw;
		return 1;
	}
	else
		return 0;
}


const char RX_str[] PROGMEM={"RX:"};
const char TX_str[] PROGMEM={"TX:"};
void homescreen_telemetry_draw_template()
{
	// LCD_clear_partial();
	LCD_gotoXY(CHAR_SPACE(4),2);
	LCD_writeChar('V',OVERWRITE);

	LCD_gotoXY(CHAR_SPACE(13),2);
	LCD_writeChar('V',OVERWRITE);

	LCD_writeString_XY(0,3,pgmtoa(TX_str),OVERWRITE);

	LCD_writeString_XY(CHAR_SPACE(7),3,pgmtoa(RX_str),OVERWRITE);

}

const char no_telemetry_str[] PROGMEM={"NO TELEMETRY"};
void homescreen_telemetry_draw()
{
	if (telemetry.updated)
	{
		if(telemetry_timeout>=TELEMETRY_TIMEOUT)
			LCD_clear_row(5);
		telemetry.updated=0;
		telemetry_frame_type telemetry_copy=telemetry;//cache it, in case it gets overwritten during reading
		telemetry_timeout=0;
		LCD_writeString_XY(CHAR_SPACE(0),2,itoa_dec( telemetry_analog_convert_value(telemetry_copy.analog_A1.raw,telemetry_copy.analog_A1.range),4,1),OVERWRITE);

		LCD_writeString_XY(CHAR_SPACE(9),2,itoa_dec( telemetry_analog_convert_value(telemetry_copy.analog_A2.raw,telemetry_copy.analog_A2.range),4,1),OVERWRITE);

		LCD_writeString_XY(CHAR_SPACE(3),3,itoa(telemetry_copy.rssi_TX,3),OVERWRITE);
		LCD_writeString_XY(CHAR_SPACE(10),3,itoa(telemetry_copy.rssi_RX,3),OVERWRITE);
	}
	else
	{
		if(telemetry_timeout<TELEMETRY_TIMEOUT)
			telemetry_timeout++;

		else if(telemetry_timeout==TELEMETRY_TIMEOUT)
		{
			// LCD_clear_partial();
			LCD_writeString_XY(5,5,pgmtoa(no_telemetry_str),OVERWRITE);
			telemetry_timeout++;//so we won't redraw static text every cycle, just once
		}
	}
}


uint8_t homescreen_input_bars_set()
{
	fiftyps_periodical_call=homescreen_input_bars;
	//FIXME: draw clock when displayed after clear, even whne not running
	return 1;//we are alway able to show input bars screen
}
void homescreen_input_bars()
{
	LCD_drawBar(10,40,64,8 ,(int32_t)(IN_NORM+inputs[IN_STEER].runtime.value)/(2*IN_NORM/100),BAR_HORIZONTAL);//bottom horizontal bar
	LCD_drawBar(0 ,22,8 ,26,-(int32_t)(IN_NORM+inputs[IN_THROTTLE].runtime.value)/(2*IN_NORM/100),BAR_VERTICAL);//left verical bar
	LCD_drawBar(76,22,8 ,26,-(int32_t)(IN_NORM+inputs[IN_DR].runtime.value)/(2*IN_NORM/100),BAR_VERTICAL);//right vertical bar
	LCD_drawBar(0 ,12,20,8 ,(int32_t)(IN_NORM+inputs[IN_THUMB].runtime.value)/(2*IN_NORM/100),BAR_HORIZONTAL);//left horizontal bar
	LCD_drawBar(HW_COLUMNS-20,12,20,8,(int32_t)(IN_NORM+inputs[IN_ALT].runtime.value)/(2*IN_NORM/100),BAR_HORIZONTAL);//right horizontal bar

	if(screen.redraw)//in case of forced redraw, draw clock every time
		clock_draw();
	else
		clock_draw_cond();
}

//here ends custom homescreens setters and drawers-------------------------


//homescreen button handlers, changers etc.
void homescreen_button_listener(uint8_t buttons)
{
	switch (buttons) {
		case BTN_FUNC_LONG:
			menu_start();
			break;
		case BTN_SEL:
			clock_toggle();
			break;
		case BTN_SEL_LONG:
			clock_reset();
			break;
		case BTN_INC:
			homescreen_next();
			break;
		case BTN_DEC:
			homescreen_prev();
			break;
	}
}

uint8_t homescreen_next_num(uint8_t new_homescreen_num)
{
	if(array_length(homescreens)-1 > new_homescreen_num)
		return new_homescreen_num+1;
	else
		return 0;
}

uint8_t homescreen_prev_num(uint8_t new_homescreen_num)
{
	if(new_homescreen_num>0)
		return new_homescreen_num-1;
	else
		return array_length(homescreens)-1;
}

void homescreen_switch(int8_t step)
{
	uint8_t new_homescreen_num=current_homescreen_num;

	if (step<0)
		new_homescreen_num=homescreen_prev_num(new_homescreen_num);
	else if(step>0)
		new_homescreen_num=homescreen_next_num(new_homescreen_num);

	for(;;)//at lleast once - do-while workaround
	{
		if( homescreens[new_homescreen_num]()!=0 )//try to set new homescreen
		{
			current_homescreen_num=new_homescreen_num;
			screen.redraw=1;
			break;//new homescreen set of we looped through all entries with no success
		}

		if (step<0)
			new_homescreen_num=homescreen_prev_num(new_homescreen_num);
		else //if step >0 move to next, or if setting current homescreen failed move to next
			new_homescreen_num=homescreen_next_num(new_homescreen_num);
	}
}

//called to return to homescreen (eg from menu)

void homescreen_return()
{
	menu_state=MENU_STATE_IDLE;
	//oneps_periodical_call=clock_draw;
	homescreen_switch(0);
	set_event_listener(homescreen_button_listener);
	set_new_drawer(homescreen_draw);
}

void homescreen_draw()
{
	screen_template_draw();
	homescreen_template_draw();
	homescreens[current_homescreen_num]();//call setter again, he might draw template
	if(fiftyps_periodical_call!=NULL)
		fiftyps_periodical_call();
}

//homescreen global templates for top bar and so...
void homescreen_template_draw()
{
	LCD_write_header_text(model_name);
}

