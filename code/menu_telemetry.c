// Copyright (C) 2016  xorly

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

#include "menu_telemetry.h"
#include <inttypes.h>
#include <avr/pgmspace.h>
#include "3310_routines.h"
#include "menu.h"
#include "telemetry.h"
// #include "storage.h"

// void menu_output_epa_min_start(void)
// {
//
// }
//
// char *menu_output_epa_min_getval(uint8_t unused)
// {
//
// }

void menu_telemetry_toggle(void)
{
	telemetry.enabled^=1;
	menu_return();//immediately return to menu after toggling
}

char *menu_telemetry_get_val(uint8_t val)
{
	return pgmtoa(telemetry.enabled ? messages[ON_STR]:messages[OFF_STR]);
}


void menu_telemetry_analog_A1_start(void)
{
	menu_simple_lister_start(&telemetry.analog_A1.range,0, TELEMETRY_ANALOG_MAX_RANGE, menu_telemetry_analog_draw);
	menu_tmp.u8p=&telemetry.analog_A1.raw;
}

char* menu_telemetry_analog_A1_get_val(uint8_t unused)
{
	return itoa_dec(telemetry.analog_A1.range,MENU_NUMBER_LENGTH,1);
}

void menu_telemetry_analog_A2_start(void)
{
	menu_simple_lister_start(&telemetry.analog_A2.range,0, TELEMETRY_ANALOG_MAX_RANGE, menu_telemetry_analog_draw);
	menu_tmp.u8p=&telemetry.analog_A2.raw;
}

char* menu_telemetry_analog_A2_get_val(uint8_t unused)
{
	return itoa_dec(telemetry.analog_A2.range,MENU_NUMBER_LENGTH,1);
}

//editing drawer for both analog ins
void menu_telemetry_analog_draw(uint8_t val)
{
	LCD_writeString_XY(CHAR_WIDTH*4,MENU_ACTIVE_LINE,itoa_dec(telemetry_analog_convert_value((*menu_tmp.u8p),val),MENU_NUMBER_LENGTH,1),OVERWRITE|INVERT);
	LCD_writeString_XY(MENU_VALUE_COLUMN,MENU_ACTIVE_LINE,itoa_dec(val,MENU_NUMBER_LENGTH,1),OVERWRITE|INVERT);
}
