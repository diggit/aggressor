// Copyright (C) 2014  xorly

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.


#include "control.h"
#include "hardware.h"
#include "misc.h"
#include <avr/io.h>

void (*button_event_listener)(uint8_t)=NULL;
void (*oneps_periodical_call)(void)=NULL;
void (*fiftyps_periodical_call)(void)=NULL;



uint8_t buttons_pressed=0;

uint8_t buttons_pressed_now,buttons_pressed_previously;//only for local edge detecting

uint8_t long_press_inc,long_press_dec,long_press_sel,long_press_func;

uint8_t long_press_released;

void control_eval_buttons(void)//checks for pressed buttons, if some newly pressed, calls listener
{
	buttons_pressed_now=0;//clear before writing fresh BTN states (ones)
	long_press_released=0;
	if(PINB&(1<<1))//SEL
	{
		buttons_pressed_now|=BTN_SEL;
		if(long_press_sel<LONG_PRESS_THRESH)
			long_press_sel++;
		else
			buttons_pressed_now|=BTN_SEL_LONG;
	}
	else
	{
		if(long_press_sel==LONG_PRESS_THRESH)
			long_press_released=1;
		long_press_sel=0;
	}

	if(PINB&(1<<2))//FUNC
	{
		buttons_pressed_now|=BTN_FUNC;
		if(long_press_func<LONG_PRESS_THRESH)
			long_press_func++;
		else
			buttons_pressed_now|=BTN_FUNC_LONG;
	}
	else
	{
		if(long_press_func==LONG_PRESS_THRESH)
			long_press_released=1;
		long_press_func=0;
	}

	if(PINB&(1<<3))//DEC
	{
		buttons_pressed_now|=BTN_DEC;
		if(long_press_dec<LONG_PRESS_THRESH)
			long_press_dec++;
		else
			buttons_pressed_now|=BTN_DEC_LONG;
	}
	else
	{
		if(long_press_dec==LONG_PRESS_THRESH)
			long_press_released=1;
		long_press_dec=0;
	}

	if(PINC&(1<<6))//INC
	{
		buttons_pressed_now|=BTN_INC;
		if(long_press_inc<LONG_PRESS_THRESH)
			long_press_inc++;
		else
			buttons_pressed_now|=BTN_INC_LONG;
	}
	else
	{
		if(long_press_inc==LONG_PRESS_THRESH)
			long_press_released=1;
		long_press_inc=0;
	}

	buttons_pressed = buttons_pressed_now & ~buttons_pressed_previously;//detect rising edges

	if(button_event_listener!=NULL && ( buttons_pressed || long_press_released ) )
	//if some rising edge encountered or released long pressed button
	{
		if(config.beep_keys)
		{
			if(buttons_pressed&(BTN_INC_LONG|BTN_DEC_LONG|BTN_SEL_LONG|BTN_FUNC_LONG))
				beep(BEEP_KEY_LONG);
			else if(buttons_pressed&(BTN_INC|BTN_DEC|BTN_SEL|BTN_FUNC))
				beep(BEEP_KEY_COMMON);
		}
		button_event_listener(buttons_pressed);

	}
	buttons_pressed_previously=buttons_pressed_now;//make old state from actual one
}
