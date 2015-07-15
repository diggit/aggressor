// Copyright (C) 2014   xorly

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


#ifndef _CONTROL_LIB
	#define _CONTROL_LIB

	#include <inttypes.h>

	#include "config.h"
	#include "misc.h"

	//buttons
	#define BTN_NONE		0
	#define BTN_FUNC		(1<<0)
	#define BTN_DEC			(1<<1)
	#define BTN_INC			(1<<2)
	#define BTN_SEL			(1<<3)
	#define BTN_FUNC_LONG	(1<<4)
	#define BTN_DEC_LONG	(1<<5)
	#define BTN_INC_LONG	(1<<6)
	#define BTN_SEL_LONG	(1<<7)

	#define LONG_PRESS_OFFSET	4	//add to normal value to get _LONG value eg. BTN_INC+LONG_PRESS_OFFSET==BTN_INC_LONG
	#define LONG_PRESS_THRESH	20 //*20ms

	extern void (*oneps_periodical_call)(void);
	extern void (*button_event_listener)(uint8_t);//accepts button code as parameter
	extern void (*fiftyps_periodical_call)(void);

	#define set_event_listener(V) button_event_listener=V
	#define reset_event_listener() button_event_listener=NULL
	#define is_any_short(V) (V&0x0f) //lower 4 bits are short press flags

	void control_eval_buttons(void);


#endif
