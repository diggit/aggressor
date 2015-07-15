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


#include "analog.h"
#include "misc.h"
#include <avr/io.h>

volatile uint16_t adc_measured[ADC_channel_count];
uint8_t adc_battery_voltage;


uint16_t adc_battery_max=VCC_MAX_VOLTAGE_mV;//max measureable voltage in mV


//setup adc, one shot, slow,
void adc_init()
{
	//setup 10bit low sample rate, ext. ref.(def),
	ADMUX=0;//some default input selection

	//for 10bit effective, ADC freq. should be between 50kHz and 200kHz, HEY it does matter!
	//enable ADC, 64 prescaler
	ADCSRA=BIT(ADEN)|BIT(ADPS2)|BIT(ADPS1)|BIT(ADPS0);
}

//gel value from ADC, takes input number as arg., returns 0~1023, 1024 means error
uint16_t adc_raw_measure(uint8_t input)
{
	uint16_t adc_raw=0;
	ADMUX=input;
	uint8_t sample;
	for(sample=0;sample<ADC_SAMPLES;sample++)
	{
		ADCSRA|=(1<<ADSC);//convert
		while(!(ADCSRA & (1<<ADIF)))//wait, until conversion complete
			NOP;

		adc_raw+=ADC;

	}
	adc_raw/=ADC_DIVIDER;
	return adc_raw;
}

uint16_t adc_raw_buffered(uint8_t input_channel)
{
	return adc_measured[input_channel];
}

void adc_measure_all()
{
	uint8_t input;
	for(input=0;input<ADC_channel_count;input++)
	{
		adc_measured[input]=adc_raw_measure(input);
	}
	adc_battery_voltage=(uint32_t)adc_battery_max*(uint32_t)adc_measured[ADC_BATTERY]/ADC_MAX/100;
}
