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

const char *input_names [HW_INPUTS]=
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
			.adc_channel=ADC_STEER,
			.type=IN_TYPE_ANALOG,
			.raw_min=76,
			.raw_max=982,
			.raw_center=527,
			.enabled=1,
			.invert=1
		},
		.model=
		{
			.shared.deadzone=10,
			.trim=TRIM_TOP,
		}
	},
	[IN_THROTTLE]=
	{
		.global=
		{
			.adc_channel=ADC_THROTTLE,
			.type=IN_TYPE_ANALOG,
			.raw_min=386,
			.raw_max=847,
			.raw_center=538,
			.enabled=1
		},
		.model=
		{
			.shared.deadzone=2,
			.trim=TRIM_RIGHT
		}
	},
	[IN_DR]=
	{
		.global=
		{
			.adc_channel=ADC_DR,
			.type=IN_TYPE_ANALOG,
			.raw_min=0,
			.raw_max=1023,
			.raw_center=511,
			.enabled=1
		},
		.model=
		{
			.shared.deadzone=2,
			.trim=TRIM_LEFT
		}
	},
	[IN_ALT]=
	{
		.global=
		{
			.adc_channel=ADC_ALT,
			.type=IN_TYPE_LEVELING,
			// .raw_min=0,
			// .raw_max=0,
			// .raw_center=0,
			.enabled=1
		},
		.runtime=
		{
			.levels=in_alt_levels
		},
		.model=
		{
			.trim=TRIM_NONE,
			//.level_actual=0
			.shared.level_count=5
		}
	},
	[IN_THUMB]=
	{
		.global=
		{
			.adc_channel=(1<<0), //on PB0
			.type=IN_TYPE_TOGGLING,
			// .raw_min=0,
			// .raw_max=0,
			// .raw_center=0,
			.enabled=1
		},
		.model=
		{
			.trim=TRIM_NONE
		}
	}
};

uint8_t in_digital_was,in_digital_now; //for rising edge catching

void input_eval_all(void)
{
	uint8_t in_num;
	int8_t trim;
	in_digital_now=0;
	input_p in;
	for(in_num=0;in_num<array_length(inputs);in_num++)
	{
		in=&inputs[in_num];

		//this way, we could preserve trim orientation, even if input gets reversed, this is simpler way than using more formulas
		if(in->global.invert)
			trim=-in->model.trim_val;
		else
			trim=in->model.trim_val;

		if(trims[in->model.trim].inc)
		{
			if(in->model.trim_val < TRIM_MAX )
				in->model.trim_val++;
			menu_input_notify_trim_change(in,in_num);
		}
		else if(trims[in->model.trim].dec)
		{
			if(in->model.trim_val > TRIM_MIN )
				in->model.trim_val--;
			menu_input_notify_trim_change(in,in_num);
		}

		if(in->global.enabled)
		{
			if(in->global.type==IN_TYPE_ANALOG)
			{
				in->runtime.raw_value = adc_measured[in->global.adc_channel];

				if(uabs((int16_t)in->global.raw_center - (int16_t)in->runtime.raw_value) > in->model.shared.deadzone)//out of deadzone
				{
					uint16_t val=crop(in->runtime.raw_value,in->global.raw_min,in->global.raw_max);

					if(val > in->global.raw_center)//positive
						in->runtime.value=\
						(int32_t)(IN_NORM - (trim * INPUT_TRIM_MULTIPLIER))\
						*(val - in->global.raw_center - in->model.shared.deadzone)\
						/(in->global.raw_max - in->global.raw_center -  in->model.shared.deadzone)\
						+ (trim * INPUT_TRIM_MULTIPLIER);
					else //negative
						in->runtime.value=\
						-((int32_t)(IN_NORM + (trim * INPUT_TRIM_MULTIPLIER))\
						*(in->global.raw_center - val - in->model.shared.deadzone )\
						/(in->global.raw_center - in->global.raw_min - in->model.shared.deadzone)\
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
			else if(in->global.type==IN_TYPE_TOGGLING)
			{
				if( bit_get(IN_DIGITAL_PINREG , in->global.adc_channel) )
				{
					if(in->global.raw_center==0)
					{
						beep(BEEP_IN_COMMON);
						in->global.raw_center=1;
						if(in->model.level_actual==0)
							in->model.level_actual=1;
						else
							in->model.level_actual=0;
					}
					//else pressed but not edge
				}
				else
					in->global.raw_center=0;

				if(in->model.level_actual)
					in->runtime.raw_value=in->global.raw_max;
				else
					in->runtime.raw_value=in->global.raw_min;

				if(in->runtime.raw_value>0)
					in->runtime.value= (int32_t)(IN_NORM ) * in->runtime.raw_value / IN_DIGITAL_MAX;
				else
					in->runtime.value= (int32_t)(IN_NORM ) * in->runtime.raw_value / IN_DIGITAL_MAX;

			}
			else if(in->global.type==IN_TYPE_LEVELING)
			{
				in->runtime.raw_value=adc_measured[in->global.adc_channel];
				if(in->runtime.raw_value > ADC_DIGITAL_HIGH)//INC pressed
				{
					if(bit_get(in->global.raw_center,(1<<0)) ==0) //and was zero ->this is rising edge
					{
						//INC rising edge
						bit_set(in->global.raw_center, (1<<0));//remember, that INC is pressed
						if(in->model.level_actual < IN_LEVELING_MAX_LEVELS-1 && in->model.level_actual < in->model.shared.level_count-1)//if in range, increase
						{
							beep(BEEP_IN_COMMON);
							in->model.level_actual++;
							if(menu==&leveling_input_menu && menu->parent->parent->hilighted == in_num)
								menu_draw();
						}
						else
							beep(BEEP_IN_BOUNDS);
					}
				}
				else
				{
					bit_clr(in->global.raw_center, (1<<0));//remember, that INC in NOT pressed

					if(in->runtime.raw_value < ADC_DIGITAL_LOW)//DEC pressed
					{
						if(bit_get(in->global.raw_center,(1<<1)) ==0) //and was zero ->this is rising edge
						{
							//INC rising edge
							bit_set(in->global.raw_center, (1<<1));//remember, that DEC is pressed
							if(in->model.level_actual > 0)//if in range, decrease
							{
								beep(BEEP_IN_COMMON);
								in->model.level_actual--;
								if(menu==&leveling_input_menu && menu->parent->parent->hilighted == in_num)
									menu_draw();
							}
							else
								beep(BEEP_IN_BOUNDS);
						}
					}
					else
						bit_clr(in->global.raw_center, (1<<1));

				}
				if(in->runtime.levels[in->model.level_actual]>0)
					in->runtime.value= (int32_t)(IN_NORM - (in->model.trim_val * INPUT_TRIM_MULTIPLIER) ) * in->runtime.levels[in->model.level_actual]/ IN_DIGITAL_MAX + (in->model.trim_val * INPUT_TRIM_MULTIPLIER);
				else
					in->runtime.value= (int32_t)(IN_NORM - (in->model.trim_val * INPUT_TRIM_MULTIPLIER) ) * in->runtime.levels[in->model.level_actual]/ IN_DIGITAL_MAX - (in->model.trim_val * INPUT_TRIM_MULTIPLIER);



			}

			if (in->global.invert)
				in->runtime.value=-in->runtime.value;
		}
	}
}
