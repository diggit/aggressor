#include "output.h"
#include "menu.h"
#include "input.h"
#include "ppm.h"
#include "misc.h"

const char ch1_str[] PROGMEM={"CH1"};
const char ch2_str[] PROGMEM={"CH2"};
const char ch3_str[] PROGMEM={"CH3"};
const char ch4_str[] PROGMEM={"CH4"};
const char ch5_str[] PROGMEM={"CH5"};

const char *output_names[PPM_OUTPUT_COUNT]=
{
	[OUTPUT_CH1]=ch1_str,
	[OUTPUT_CH2]=ch2_str,
	[OUTPUT_CH3]=ch3_str,
	[OUTPUT_CH4]=ch4_str,
	[OUTPUT_CH5]=ch5_str
};



output_t outputs[];/* ={
	[OUTPUT_CH1]=
	{
		.subtrim_val=0,
		.epa_min=OUTPUT_EPA_MIN,
		.epa_max=OUTPUT_EPA_MAX,
		.value=1500,
		.invert=0,
		.in_coef=
			{
				[IN_STEER]=100
			}
	},
	[OUTPUT_CH2]=
	{
		.subtrim_val=0,
		.epa_min=OUTPUT_EPA_MIN,
		.epa_max=OUTPUT_EPA_MAX,
		.value=1500,
		.invert=0,
		.in_coef=
			{
				[IN_THROTTLE]=100
			}
	},
	[OUTPUT_CH3]=
	{
		.subtrim_val=0,
		.epa_min=OUTPUT_EPA_MIN,
		.epa_max=OUTPUT_EPA_MAX,
		.value=1500,
		.invert=0,
		.in_coef=
			{
				[IN_DR]=100
			}
	},
	[OUTPUT_CH4]=
	{
		.subtrim_val=0,
		.epa_min=OUTPUT_EPA_MIN,
		.epa_max=OUTPUT_EPA_MAX,
		.value=1500,
		.invert=0,
		.in_coef=
			{
				[IN_ALT]=100
			}
	},
	[OUTPUT_CH5]=
	{
		.subtrim_val=0,
		.epa_min=OUTPUT_EPA_MIN,
		.epa_max=OUTPUT_EPA_MAX,
		.value=1500,
		.invert=0,
		.in_coef=
			{
				[IN_THUMB]=100
			}
	}
};*/

void output_load_defaults(void)
{
	// int8_t	epa_min;
	// int8_t	epa_max;
	// uint16_t	value;
	// int8_t	in_coef[IN_COUNT];
	// uint8_t invert:1;

	for (size_t ch = 0; ch < array_length(outputs); ch++) {
		outputs[ch].subtrim_val=0;
		outputs[ch].epa_min=OUTPUT_EPA_MIN;
		outputs[ch].epa_max=OUTPUT_EPA_MAX;
		outputs[ch].value=1500;
		for (size_t coef = 0; coef < IN_COUNT; coef++) //bind first in to first out and so on
		{
			if(ch==coef)
				outputs[ch].in_coef[coef]=10;
			else
				outputs[ch].in_coef[coef]=0;
		}
	}

}

void output_eval_all()
{
	output_p out;
	int32_t sum_buffer=0;
	int16_t divider=0;
	int8_t epa;
	for (uint8_t out_num = 0; out_num < array_length(outputs); out_num++)
	{
		out=&outputs[out_num];

		sum_buffer=0;
		divider=0;

		for (uint8_t in_num = 0; in_num < IN_COUNT; in_num++)
		{
			divider+=uabs(out->in_coef[in_num]);

			if(out->in_coef[in_num]!=0) //if coef==0, input value doesn't matter, will add 0
			{
				//TODO: handle negative maximal and positive minimal EPA properly
				// if(inputs[in_num].runtime.value >=0) //use positive EPA for potitive value
				// {
				// 	//					in_value[0~2000]	*	500us * pos_epa[0~100]	/	(2000	*	100)
				// 	sum_buffer +=  ((int32_t)inputs[in_num].runtime.value * (PPM_PULSE_DEFLECTION * PPM_ONE_US) * out->epa_max / ((int32_t)IN_NORM * OUTPUT_EPA_MAX))  * (int32_t)out->in_coef[in_num];
				// }
				// else
				// {
				// 	//epa_min and input->value are both negative and they cancels sign
				//
				// 	sum_buffer += -((int32_t)inputs[in_num].runtime.value * (PPM_PULSE_DEFLECTION * PPM_ONE_US) * out->epa_min / ((int32_t)IN_NORM * OUTPUT_EPA_MAX))  * (int32_t)out->in_coef[in_num];
				// }
				sum_buffer +=(int32_t)inputs[in_num].runtime.value * (int32_t)out->in_coef[in_num];

			}
		}
		if(sum_buffer>=0)
			epa=out->epa_max;
		else
			epa=-out->epa_min;

		out->value = (PPM_PULSE_CENTER * PPM_ONE_US) + (out->subtrim_val * OUTPUT_SUBTRIM_MULTIPLIER)+\
		(int32_t)(PPM_PULSE_DEFLECTION * PPM_ONE_US)*sum_buffer / divider * epa / ((int32_t)OUTPUT_EPA_MAX*IN_NORM) ;//this line could overflow in case of too high constants, parts that can overflow...
		//(PPM_PULSE_DEFLECTION * PPM_ONE_US)*sum_buffer
		//and/or
		//(PPM_PULSE_DEFLECTION * PPM_ONE_US)*sum_buffer / divider * epa
		//highest value of sum_buffer is (IN_NORM*OUTPUT_INMIX_COEF_MAX*IN_COUNT)
	}
}

//copy time value of each channel to ppm register (after this, pulses are updated)
void output_push_to_ppm()
{
	for (uint8_t out_num = 0; out_num < array_length(outputs); out_num++)
	{
		ppm_output[out_num] = outputs[out_num].value;
	}
}
