#include "menu_inputs.h"
#include "3310_routines.h"
#include "hardware.h"
#include "input.h"
#include "menu.h"
#include "trim.h"
#include "control.h"
#include "systick.h"

#define SELECTED menu->parent->hilighted

const char idle_trim_str[] PROGMEM={"trim "};
const char not_available_str[] PROGMEM={"N/A"};

void menu_input_notify_trim_change(input_p in,uint8_t in_num)
{
	if(in->model.trim_val==0)
		beep(BEEP_TRIM_ZERO);
	else
		beep(BEEP_TRIM_COMMON);

	if(menu_state==MENU_STATE_IDLE)
	{
		LCD_writeString_XY(10,3,pgmtoa(idle_trim_str),OVERWRITE);
		LCD_writeString_XY(0,0,pgmtoa(input_names[in_num]),RELATIVE|OVERWRITE);
		LCD_writeString_XY(25,4,itoa(in->model.trim_val,3),OVERWRITE);
		screen_schedule_redraw(2*SCREEN_REDRAW_1S);
	}
}


//TRIM bind menu functions
void menu_input_trim_start(void)
{
	if(inputs[SELECTED].global.type!=IN_TYPE_ANALOG)
	{
		//nothig to do for non analog input
		menu_return();
	}
	else
	{
		menu_tmp.u8=inputs[menu->parent->hilighted].model.trim;
		menu_simple_lister_start_ex(&menu_tmp.u8,0, array_length(trims)-1, menu_input_trim_draw,menu_input_trim_flush);
	}
}

void menu_input_trim_draw(uint8_t val)
{
	menu_items_draw_value(MENU_ACTIVE_LINE,pgmtoa(trims[val].name),OVERWRITE|INVERT);
}

char* menu_input_trim_getval(uint8_t unused)
{
	if(inputs[SELECTED].global.type!=IN_TYPE_ANALOG)
		return pgmtoa(not_available_str);
	else
		return pgmtoa(trims[inputs[menu->parent->hilighted].model.trim].name);
}

void menu_input_trim_flush(void)
{
	inputs[menu->parent->hilighted].model.trim=menu_tmp.u8;
}



void menu_input_callibration_start(void)
{
	if(inputs[SELECTED].global.type==IN_TYPE_LEVELING)
	{
		menu_enter_submenu(&leveling_input_menu);
	}
	else if(inputs[SELECTED].global.type==IN_TYPE_TOGGLING)
	{
		menu_enter_submenu(&_input_menu);
	}
	else if(inputs[SELECTED].global.type==IN_TYPE_ANALOG)
	{
		menu_input_callibration_analog_start();
	}
	else
	{
		menu_return();
	}
}

void menu_input_callibration_analog_start()
{
	LCD_clear_partial();
	//LCD_update();
	set_event_listener(menu_input_callibration_analog_listener);
	set_new_drawer(menu_input_callibration_analog_draw);
}

uint16_t new_min,new_max;

void menu_input_callibration_analog_draw_values()
{
	uint16_t raw=inputs[SELECTED].runtime.raw_value;
	if(new_min>raw)
		new_min=raw;
	if(new_max<raw)
		new_max=raw;
	LCD_writeString_XY(23,3,itoa(new_min,4),1);
	LCD_writeString_XY(23,4,itoa(new_max,4),1);
	LCD_writeString_XY(23,5,itoa(inputs[SELECTED].runtime.raw_value,4),1);
}

void menu_input_callibration_analog_draw_template()
{
	new_min=65535;
	new_max=0;
	LCD_writeString_XY(40,2,"ANALOG",1);
	LCD_writeString_XY(0,3,"MIN:",1);
	LCD_writeString_XY(0,4,"MAX:",1);
	LCD_writeString_XY(0,5,"MID:",1);

	LCD_writeString_XY(54,3,itoa(inputs[SELECTED].global.raw_min,4),1);
	LCD_writeString_XY(54,4,itoa(inputs[SELECTED].global.raw_max,4),1);
	LCD_writeString_XY(54,5,itoa(inputs[SELECTED].global.raw_center,4),1);

	fiftyps_periodical_call=menu_input_callibration_analog_draw_values;
}

void menu_input_callibration_analog_listener(uint8_t buttons)
{
	if(is_any_short(buttons))
	{
		if(buttons&BTN_SEL)
		{
			inputs[SELECTED].global.raw_max=new_max;
			inputs[SELECTED].global.raw_min=new_min;
			if(inputs[SELECTED].runtime.raw_value == inputs[SELECTED].global.raw_max || inputs[SELECTED].runtime.raw_value == inputs[SELECTED].global.raw_min)
				//calculate center
				inputs[SELECTED].global.raw_center=(inputs[SELECTED].global.raw_max-inputs[SELECTED].global.raw_min)/2+inputs[SELECTED].global.raw_min;
			else
				inputs[SELECTED].global.raw_center=inputs[SELECTED].runtime.raw_value;

		}
		menu_input_callibration_analog_draw();
	}
	else if(buttons&BTN_FUNC_LONG)
		menu_return();//return to menu
}

void menu_input_callibration_analog_draw()
{
	fiftyps_periodical_call=NULL;
	LCD_clear_rows((1<<2)|(1<<3)|(1<<4)|(1<<5));

	menu_input_callibration_analog_draw_template();

	LCD_writeString_XY(0,2,pgmtoa(input_names[menu->parent->hilighted]),OVERWRITE);

}


//DEADZONE setup
void menu_input_deadzone_start()
{
	if(inputs[menu->parent->hilighted].global.type==IN_TYPE_ANALOG) //only for analog inputs makes deadzone sense
		menu_simple_lister_start(&inputs[menu->parent->hilighted].model.shared.deadzone,0, 100 , NULL);
	else
		menu_return();
}

char *menu_input_deadzone_getval(uint8_t unused)
{
	if(inputs[menu->parent->hilighted].global.type==IN_TYPE_ANALOG) //only for analog inputs makes deadzone sense
		return itoa(inputs[menu->parent->hilighted].model.shared.deadzone,MENU_NUMBER_LENGTH);
	else
		return pgmtoa(not_available_str);
}


void menu_input_enabled_toggle()
{
	inputs[menu->parent->hilighted].global.enabled ^=1;
	menu_return();
}
char *menu_input_enabled_getval(uint8_t unused)
{
	return pgmtoa(inputs[menu->parent->hilighted].global.enabled ? messages[ON_STR] : messages[OFF_STR]);
}


void menu_input_invert_toggle()
{
	inputs[menu->parent->hilighted].global.invert ^=1;
	menu_return();
}
char *menu_input_invert_getval(uint8_t unused)
{
	return pgmtoa(inputs[menu->parent->hilighted].global.invert ? messages[ON_STR] : messages[OFF_STR]);
}

// void menu_input_STH_start(void)
// {
//
// }
//
// void menu_input_STH_draw(uint8_t val)
// {
//
// }
// char *menu_input_STH_getval(uint8_t unused)
// {
//
// }

// LEVELS of leveling input
void menu_input_leveling_max_levels_start(void)
{
	menu_simple_lister_start_ex(&inputs[menu->parent->parent->hilighted].model.shared.level_count,2 , IN_LEVELING_MAX_LEVELS , NULL, menu_input_leveling_max_levels_flush);
}

void menu_input_leveling_max_levels_flush(void)
{
	input_p in=&inputs[menu->parent->parent->hilighted];
	in->model.level_actual=crop(in->model.level_actual,0,in->model.shared.level_count-1); //if actual lvl is higher than max, decrease it
}

char *menu_input_leveling_max_levels_getval(uint8_t unused)
{
	return itoa(inputs[menu->parent->parent->hilighted].model.shared.level_count,MENU_NUMBER_LENGTH);
}

//actual level value of leveling
void menu_input_leveling_actual_level_start(void)
{
	menu_simple_lister_signed_start(&inputs[menu->parent->parent->hilighted].runtime.levels[ inputs[menu->parent->parent->hilighted].model.level_actual ],IN_DIGITAL_MIN , IN_DIGITAL_MAX , menu_input_leveling_actual_level_draw);
}

char *menu_input_leveling_actual_level_getval(uint8_t unused)
{
	return itoa(inputs[menu->parent->parent->hilighted].runtime.levels[ inputs[menu->parent->parent->hilighted].model.level_actual ],MENU_NUMBER_LENGTH);
}

void menu_input_leveling_actual_level_draw(int8_t val)
{
	input_p in=&inputs[menu->parent->parent->hilighted];
	*lister_original_value_p.s=lister_value_copy.s;//save value to take effect immediately on output

	if(lister_original_value_p.s != &in->runtime.levels[ in->model.level_actual ] )//level changed during edit
	{
		lister_original_value_p.s=&in->runtime.levels[ in->model.level_actual ]; //refresh pointer to value being changed
		lister_value_copy.s=*lister_original_value_p.s;//copy actual value to copy variable
	}
	//finally, draw itoa
	menu_items_draw_value(MENU_ACTIVE_LINE,itoa(lister_value_copy.s,MENU_NUMBER_LENGTH),OVERWRITE|INVERT);
}

//
void menu_input_toggling_min_start(void)
{
	menu_tmp.s8=inputs[menu->parent->parent->hilighted].global.raw_min;
	menu_simple_lister_signed_start_ex(&menu_tmp.s8,IN_DIGITAL_MIN, inputs[menu->parent->parent->hilighted].global.raw_max-1 , NULL, menu_input_toggling_min_flush);
}

void menu_input_toggling_max_start(void)
{
	menu_tmp.s8=inputs[menu->parent->parent->hilighted].global.raw_max;
	menu_simple_lister_signed_start_ex(&menu_tmp.s8,inputs[menu->parent->parent->hilighted].global.raw_min+1, IN_DIGITAL_MAX , NULL, menu_input_toggling_max_flush);

}
void menu_input_toggling_min_flush(void)
{
	inputs[menu->parent->parent->hilighted].global.raw_min=menu_tmp.s8;
}

void menu_input_toggling_max_flush(void)
{
	inputs[menu->parent->parent->hilighted].global.raw_max=menu_tmp.s8;
}

char *menu_input_toggling_max_getval(uint8_t unused)
{
	return itoa(inputs[menu->parent->parent->hilighted].global.raw_max,MENU_NUMBER_LENGTH);
}

char *menu_input_toggling_min_getval(uint8_t unused)
{
	return itoa(inputs[menu->parent->parent->hilighted].global.raw_min,MENU_NUMBER_LENGTH);
}
