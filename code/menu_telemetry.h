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

#include <inttypes.h>

void menu_telemetry_toggle(void);
char *menu_telemetry_get_val(uint8_t val);

void menu_telemetry_analog_A1_start(void);
char* menu_telemetry_analog_A1_get_val(uint8_t unused);
void menu_telemetry_analog_A2_start(void);
char* menu_telemetry_analog_A2_get_val(uint8_t unused);
void menu_telemetry_analog_draw(uint8_t val);