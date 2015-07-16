#include "input.h"
#include "analog.h"
#include "misc.h"
#include "hardware.h"
#include "menu.h"
#include "trim.h"
#include "menu_inputs.h"


int8_t in_alt_levels[IN_LEVELING_MAX_LEVELS]={-100,-50,0,50,100};

//keep names separated from structure to save some space in eeprom when saving whole structures AND those pointers could change any in any new build!

const char IN_STEER_str[] PROGMEM={"WHL"};
const char IN_THROTTLE_str[] PROGMEM={"TRIG"};
const char IN_DR_str[] PROGMEM={"D/R"};
const char IN_ALT_str[] PROGMEM={"ALT"};
const char IN_THUMB_str[] PROGMEM={"THMB"};

const char *input_names [IN_COUNT]=
{
	[IN_STEER]=IN_STEER_str,
	[IN_THROTTLE]=IN_THROTTLE_str,
	[IN_DR]=IN_DR_str,
	[IN_ALT]=IN_ALT_str,
	[IN_THUMB]=IN_THUMB_str
};

input inputs[]=
{
	[IN_STEER]=
	{
		.global=
		{
			.analog=
			{
				.adc_channel=ADC_STEER,
				.raw_min=76,
				.raw_max=982,
				.raw_center=527
			},
			.common=
			{
				.type=IN_TYPE_ANALOG,
				.enabled=1,
				.invert=1
			}
		},
		.model=
		{
			.analog=
			{
				.deadzone=10,
				.trim=TRIM_TOP,
			}
		}
	},
	[IN_THROTTLE]=
	{
		.global=
		{
			.analog=
			{
				.adc_channel=ADC_THROTTLE,
				.raw_min=386,
				.raw_max=847,
				.raw_center=538
			},
			.common=
			{
				.type=IN_TYPE_ANALOG,
				.enabled=1
			}
		},
		.model=
		{
			.analog=
			{
				.deadzone=2,
				.trim=TRIM_RIGHT
			}
		}
	},
	[IN_DR]=
	{
		.global=
		{
			.analog=
			{
				.adc_channel=ADC_DR,
				.raw_min=0,
				.raw_max=1023,
				.raw_center=511
			},
			.common=
			{
				.type=IN_TYPE_ANALOG,
				.enabled=1
			}
		},
		.model=
		{
			.analog=
			{
				.deadzone=2,
				.trim=TRIM_LEFT
			}
		}
	},
	[IN_ALT]=
	{
		.global=
		{
			.analog=
			{
				.adc_channel=ADC_ALT,//yeah, this is digital channel, but control buttons are connected to ADC
			},
			.common=
			{
				.type=IN_TYPE_LEVELING,
				.enabled=1
			}
		},
		.runtime=
		{
			.levels=in_alt_levels
		},
		.model=
		{
			.digital=
			{
				.level_count=5
			}
		}
	},
	[IN_THUMB]=
	{
		.global=
		{
			.digital=
			{
				.pin=(1<<0) //on PB0
			},
			.common=
			{
				.type=IN_TYPE_TOGGLING,
				.enabled=1
			}
			// .raw_min=0,
			// .raw_max=0,
			// .raw_center=0,
		}
	}
};

uint8_t in_digital_was,in_digital_now; //for rising edge catching

void input_eval_all(void)
{
	uint8_t in_num;
	in_digital_now=0;
	input_p in;
	for(in_num=0;in_num<array_length(inputs);in_num++)
	{
		in=&inputs[in_num];


		if(in->global.common.enabled)
		{
			if(in->global.common.type==IN_TYPE_ANALOG)
			{
				int8_t trim;
				if(in->global.common.invert) //this way, we could preserve trim orientation, even if input gets reversed, this is simpler way than using more formulas
					trim=-in->model.analog.trim_val;
				else
					trim=in->model.analog.trim_val;

				//resolve, if any trim was pressed, change trim value
				if(trims[in->model.analog.trim].inc)
				{
					if(in->model.analog.trim_val < TRIM_MAX )
						in->model.analog.trim_val++;
					menu_input_notify_trim_change(in,in_num);
				}
				else if(trims[in->model.analog.trim].dec)
				{
					if(in->model.analog.trim_val > TRIM_MIN )
						in->model.analog.trim_val--;
					menu_input_notify_trim_change(in,in_num);
				}

				in->runtime.raw_value = adc_measured[in->global.analog.adc_channel];

				if(uabs((int16_t)in->global.analog.raw_center - (int16_t)in->runtime.raw_value) > in->model.analog.deadzone)//out of deadzone
				{
					uint16_t val=crop(in->runtime.raw_value,in->global.analog.raw_min,in->global.analog.raw_max);

					if(val > in->global.analog.raw_center)//positive
						in->runtime.value=\
						(int32_t)(IN_NORM - (trim * INPUT_TRIM_MULTIPLIER))\
						*(val - in->global.analog.raw_center - in->model.analog.deadzone)\
						/(in->global.analog.raw_max - in->global.analog.raw_center -  in->model.analog.deadzone)\
						+ (trim * INPUT_TRIM_MULTIPLIER);
					else //negative
						in->runtime.value=\
						-((int32_t)(IN_NORM + (trim * INPUT_TRIM_MULTIPLIER))\
						*(in->global.analog.raw_center - val - in->model.analog.deadzone )\
						/(in->global.analog.raw_center - in->global.analog.raw_min - in->model.analog.deadzone)\
						- (trim * INPUT_TRIM_MULTIPLIER));

						//A: maximal value of variable part(decreases with offset)
						//B:raw actual size of variable part
						//C:raw MAX size of variable part
						//D:trimmer offset
						// C/B is in range 0~1, *A gives it propper scale and +D propper offset
				}
				else
					in->runtime.value=(trim * INPUT_TRIM_MULTIPLIER);//otherwise, use trim offset only


			}
			else if(in->global.common.type==IN_TYPE_TOGGLING)
			{
				if( bit_get(IN_DIGITAL_PINREG , in->global.digital.pin) )
				{
					if(in->global.digital.edge_detect==0)
					{
						beep(BEEP_IN_COMMON);
						in->global.digital.edge_detect=1;
						if(in->model.digital.level_actual==0)
							in->model.digital.level_actual=1;
						else
							in->model.digital.level_actual=0;
					}
					//else pressed but not edge
				}
				else
					in->global.digital.edge_detect=0;

				if(in->model.digital.level_actual)
					in->runtime.raw_value=in->global.digital.max;
				else
					in->runtime.raw_value=in->global.digital.min;

				if(in->runtime.raw_value>0)
					in->runtime.value= (int32_t)(IN_NORM ) * in->runtime.raw_value / IN_DIGITAL_MAX;
				else
					in->runtime.value= (int32_t)(IN_NORM ) * in->runtime.raw_value / IN_DIGITAL_MAX;

			}
			else if(in->global.common.type==IN_TYPE_LEVELING)
			{
				in->runtime.raw_value=adc_measured[in->global.analog.adc_channel];
				if(in->runtime.raw_value > ADC_DIGITAL_HIGH)//INC pressed
				{
					if(bit_get(in->global.digital.edge_detect,(1<<0)) ==0) //and was zero ->this is rising edge
					{
						//INC rising edge
						bit_set(in->global.digital.edge_detect, (1<<0));//remember, that INC is pressed
						if(in->model.digital.level_actual < IN_LEVELING_MAX_LEVELS-1 && in->model.digital.level_actual < in->model.digital.level_count-1)//if in range, increase
						{
							beep(BEEP_IN_COMMON);
							in->model.digital.level_actual++;
							if(menu==&leveling_input_menu && menu->parent->parent->hilighted == in_num)
								menu_draw();
						}
						else
							beep(BEEP_IN_BOUNDS);
					}
				}
				else
				{
					bit_clr(in->global.digital.edge_detect, (1<<0));//remember, that INC in NOT pressed

					if(in->runtime.raw_value < ADC_DIGITAL_LOW)//DEC pressed
					{
						if(bit_get(in->global.digital.edge_detect,(1<<1)) ==0) //and was zero ->this is rising edge
						{
							//INC rising edge
							bit_set(in->global.digital.edge_detect, (1<<1));//remember, that DEC is pressed
							if(in->model.digital.level_actual > 0)//if in range, decrease
							{
								beep(BEEP_IN_COMMON);
								in->model.digital.level_actual--;
								if(menu==&leveling_input_menu && menu->parent->parent->hilighted == in_num)
									menu_draw();
							}
							else
								beep(BEEP_IN_BOUNDS);
						}
					}
					else
						bit_clr(in->global.digital.edge_detect, (1<<1));

				}
				in->runtime.value= (int32_t)(IN_NORM) * in->runtime.levels[in->model.digital.level_actual]/ IN_DIGITAL_MAX;
			}

			if (in->global.common.invert)
				in->runtime.value=-in->runtime.value;
		}
	}
}
