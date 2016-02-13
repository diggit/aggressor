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


#ifndef _TELEMETRY_LIB
	#define _TELEMETRY_LIB

	#include <inttypes.h>

	#include "config.h"

	#ifndef	TELEMETRY_TIMEOUT
		#define TELEMETRY_TIMEOUT	10
	#endif

	#define TELEMETRY_ANALOG_MAX	255
	#define TELEMETRY_ANALOG_MAX_RANGE	TELEMETRY_ANALOG_MAX

	typedef struct{
		volatile uint8_t raw;
		uint8_t range;
	}analog_telemetry;

	typedef struct{
		analog_telemetry analog_A1;
		analog_telemetry analog_A2;
		volatile uint8_t rssi_RX;//means siagnal strength RECEIVED BY RX
		volatile uint8_t rssi_TX;//this one for TX, is doubled
		uint8_t updated:1;
		uint8_t enabled:1;
	} telemetry_frame_type;

	telemetry_frame_type telemetry;

	uint8_t telemetry_analog_convert_value(uint8_t raw, uint8_t range);
	void telemetry_process_frame(uint8_t raw[]);


#endif