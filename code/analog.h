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


#ifndef _ANALOG_LIB
	#define _ANALOG_LIB

	#include <inttypes.h>

	#include "config.h"

	//how many should be value sampled, then averaged
	#ifndef ADC_SAMPLES
		#define ADC_SAMPLES	5
	#endif

	#ifndef ADC_DIVIDER
		#define ADC_DIVIDER ADC_SAMPLES
	#endif

	#define ADC_ALT				0
	#define ADC_TRIM_LEFT		1
	#define ADC_TRIM_RIGHT		2
	#define ADC_TRIM_TOP		3
	#define ADC_BATTERY			4
	#define ADC_DR				5
	#define ADC_THROTTLE		6
	#define ADC_STEER			7

	#define ADC_channel_count	8

	#define ADC_last_channel	ADC_STEER

	#define ADC_DIGITAL_LOW  	(100*ADC_SAMPLES/ADC_DIVIDER)
	#define ADC_DIGITAL_HIGH 	(700*ADC_SAMPLES/ADC_DIVIDER)

	#define ADC_MAX			(1024*ADC_SAMPLES/ADC_DIVIDER-1)



	//array with DEFAULT calibration values (FLASH)

	extern uint16_t adc_battery_max;

	uint8_t adc_battery_voltage;

	//last measured values
	extern volatile uint16_t adc_measured[ADC_channel_count];

	void		adc_init(void);
	uint16_t	adc_raw_measure(uint8_t input);
	uint16_t	adc_raw_buffered(uint8_t input);
	void		adc_measure_all(void);


#endif
