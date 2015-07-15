#include "trim.h"
#include "analog.h"
#include <avr/pgmspace.h>


uint8_t trims_raw=0;
uint8_t trims_now=0,trims_was=0;

const char none_str[] PROGMEM={"NONE"};
const char left_str[] PROGMEM={"LEFT"};
const char right_str[] PROGMEM={"RGHT"};
const char top_str[] PROGMEM={"TOP"};

trim_t trims[TRIM_COUNT]=
{
	[TRIM_NONE]=
	{
		.name=none_str
	},
	[TRIM_LEFT]=
	{
		.name=left_str
	},
	[TRIM_RIGHT]=
	{
		.name=right_str
	},
	[TRIM_TOP]=
	{
		.name=top_str
	}
};

void trim_read_all()
{
	trims_was=trims_now;
	trims_now=0;


	if(adc_measured[ADC_TRIM_LEFT]<ADC_DIGITAL_LOW)
		trims_now|=(1<<TRIM_LEFT_DEC); //1<<0
	else if(adc_measured[ADC_TRIM_LEFT]>ADC_DIGITAL_HIGH)
		trims_now|=(1<<TRIM_LEFT_INC); //1<<1

	if(adc_measured[ADC_TRIM_TOP]<ADC_DIGITAL_LOW)
		trims_now|=(1<<TRIM_TOP_DEC); //1<<2
	else if(adc_measured[ADC_TRIM_TOP]>ADC_DIGITAL_HIGH)
		trims_now|=(1<<TRIM_TOP_INC); //1<<3

	if(adc_measured[ADC_TRIM_RIGHT]<ADC_DIGITAL_LOW)
		trims_now|=(1<<TRIM_RIGHT_DEC); //1<<4
	else if(adc_measured[ADC_TRIM_RIGHT]>ADC_DIGITAL_HIGH)
		trims_now|=(1<<TRIM_RIGHT_INC); //1<<5


	trims_raw= trims_now & ~trims_was;

	trims[TRIM_LEFT].inc= (trims_raw>>TRIM_LEFT_INC)&1;
	trims[TRIM_LEFT].dec= (trims_raw>>TRIM_LEFT_DEC)&1;

	trims[TRIM_TOP].inc= (trims_raw>>TRIM_TOP_INC)&1;
	trims[TRIM_TOP].dec= (trims_raw>>TRIM_TOP_DEC)&1;

	trims[TRIM_RIGHT].inc= (trims_raw>>TRIM_RIGHT_INC)&1;
	trims[TRIM_RIGHT].dec= (trims_raw>>TRIM_RIGHT_DEC)&1;

	//TODO: consider for looping this ^^^
}
