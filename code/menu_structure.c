#include "menu_structure.h"
#include <avr/pgmspace.h>
#include "menu_inputs.h"
#include "menu_outputs.h"
#include "menu_simple_controls.h"
#include "menu_model.h"
#include "menu_telemetry.h"
#include "ppm.h"
#include "storage.h"
#include "misc.h"
#include "output.h"


menu_items output_menu_items[PPM_OUTPUT_COUNT];
menu_items output_inmix_items[PPM_OUTPUT_COUNT];

menu_items input_menu_items[IN_COUNT];


const char subtrim_str[] PROGMEM={"subtrim"};
const char inmix_str[] PROGMEM={"inMIX"};
const char epa_min_str[] PROGMEM={"EPA min"};
const char epa_max_str[] PROGMEM={"EPA max"};

menu_items output_propeties[]=
{
	{
		.name=subtrim_str,
		.get_value=menu_output_subtrim_getval,
		.handler_func=menu_output_subtrim_start
	},
	{
		.name=inmix_str,
		.submenu=&output_inmix_menu
	},
	{
		.name=epa_min_str,
		.handler_func=menu_output_epa_min_start,
		.get_value=menu_output_epa_min_getval
	},
	{
		.name=epa_max_str,
		.handler_func=menu_output_epa_max_start,
		.get_value=menu_output_epa_max_getval
	}
};

const char calibration_str[] PROGMEM={"calibration"};
const char deadzone_str[] PROGMEM={"deadzone"};
const char enabled_str[] PROGMEM={"enabled"};
const char invert_str[] PROGMEM={"invert"};
const char trim_str[] PROGMEM={"trim"};

menu_items input_properties[]=
{
	{
		.name=calibration_str,
		.handler_func=menu_input_callibration_start
	},
	{
		.name=trim_str,
		.get_value=menu_input_trim_getval,
		.handler_func=menu_input_trim_start
	},
	{
		.name=deadzone_str,
		.get_value=menu_input_deadzone_getval,
		.handler_func=menu_input_deadzone_start
	},
	{
		.name=enabled_str,
		.get_value=menu_input_enabled_getval,
		.handler_func=menu_input_enabled_toggle
	},
	{
		.name=invert_str,
		.get_value=menu_input_invert_getval,
		.handler_func=menu_input_invert_toggle
	}
};

const char model_str[] PROGMEM={"model"};
const char outputs_str[] PROGMEM={"outputs"};
const char inputs_str[] PROGMEM={"inputs"};
const char ppm_out_str[] PROGMEM={"PPM out"};
const char beep_str[] PROGMEM={"BEEP"};
const char beep_btn_str[] PROGMEM={"BEEP btn"};
const char lowbatt_str[] PROGMEM={"LOW BATT"};
const char telemetry_str[] PROGMEM={"tlmtry"};

menu_items main_menu_itemss[]=
{

	{
		.name=model_str,
		.submenu=&model_menu
	},
	{
		.name=outputs_str,
		.submenu=&output_menu
	},
	{
		.name=inputs_str,
		.submenu=&input_menu
	},
	{
		.name=ppm_out_str,
		.handler_func=menu_ppm_toggle,
		.get_value=menu_ppm_get_val
	},
	{
		.name=telemetry_str,
		.submenu=&telemetry_menu
	},
	{
		.name=beep_str,
		.handler_func=menu_beep_toggle,
		.get_value=menu_beep_get_val
	},
	{
		.name=beep_btn_str,
		.handler_func=menu_beep_keys_toggle,
		.get_value=menu_beep_keys_get_val
	},
	{
		.name=lowbatt_str,
		.handler_func=menu_lowbatt_start,
		.get_value=menu_lowbatt_getval,
	}//,
	// {
	// 	.name="DEE",
	// 	.handler_func=&storage_dump_eeprom
	// },
	// {
	// 	.name="EEE",
	// 	.handler_func=&storage_wipe_all
	// },
};

const char copy_to_str[] PROGMEM={"copy to"};
const char load_str[] PROGMEM={"load from"};
const char delete_str[] PROGMEM={"delete at"};
const char load_def_str[] PROGMEM={"load def!"};


menu_items model_items[]=
{
	{
		.get_value=menu_model_name_getval,
		.handler_func=menu_model_name_change
	},
	{
		.name=load_str,
		.handler_func=menu_model_load_start
	},
	{
		.name=copy_to_str,
		.handler_func=menu_model_copy_to_start
	},
	{
		.name=delete_str,
		.handler_func=menu_model_delete_start
	},
	{
		.name=load_def_str,
		.handler_func=menu_model_load_defaults
	}
};

const char levels_str[] PROGMEM={"levels"};
const char actual_level_str[] PROGMEM={"this lvl"};


menu_items leveling_input_items[]=
{
	{
		.name=levels_str,
		.get_value=menu_input_leveling_max_levels_getval,
		.handler_func=menu_input_leveling_max_levels_start
	},
	{
		.name=actual_level_str,
		.get_value=menu_input_leveling_actual_level_getval,
		.handler_func=menu_input_leveling_actual_level_start
	}
};

const char min_str[] PROGMEM={"min"};
const char max_str[] PROGMEM={"max"};


menu_items _input_items[]=
{
	{
		.name=min_str,
		.get_value=menu_input_toggling_min_getval,
		.handler_func=menu_input_toggling_min_start
	},
	{
		.name=max_str,
		.get_value=menu_input_toggling_max_getval,
		.handler_func=menu_input_toggling_max_start
	}
};

const char a1RNG_str[] PROGMEM={"A1 RNG"};
const char a2RNG_str[] PROGMEM={"A2 RNG"};

menu_items telemetry_menu_items[]=
{
	{
		.name=telemetry_str,
		.get_value=menu_telemetry_get_val,
		.handler_func=menu_telemetry_toggle
	},
	{
		.name=a1RNG_str,
		.get_value=menu_telemetry_analog_A1_get_val,
		.handler_func=menu_telemetry_analog_A1_start
	},
	{
		.name=a2RNG_str,
		.get_value=menu_telemetry_analog_A2_get_val,
		.handler_func=menu_telemetry_analog_A2_start
	}
};

const char menu_str[] PROGMEM={"MENU"};
const char outs_str[] PROGMEM={"OUTS"};
const char ins_str[] PROGMEM={"INS"};


menu_t main_menu={
	.name=menu_str,
	.items=main_menu_itemss,
	.items_total=array_length(main_menu_itemss),
	.hilighted=0,
	.parent=NULL
};

menu_t output_menu={
	.name=outs_str,
	.items=output_menu_items,
	.items_total=array_length(output_menu_items),
};

menu_t input_menu={
	.name=ins_str,
	.items=input_menu_items,
	.items_total=array_length(input_menu_items)
};

menu_t output_properties_menu={
	.items=output_propeties,
	.items_total=array_length(output_propeties)
};

menu_t input_properties_menu={
	.items=input_properties,
	.items_total=array_length(input_properties)
};

menu_t output_inmix_menu={
	.items=output_inmix_items,
	.items_total=array_length(output_inmix_items)
};

menu_t model_menu={
	.items=model_items,
	.items_total=array_length(model_items)
};

menu_t leveling_input_menu={
	.items=leveling_input_items,
	.items_total=array_length(leveling_input_items)
};

menu_t _input_menu={
	.items=_input_items,
	.items_total=array_length(_input_items)
};

menu_t telemetry_menu={
	.items=telemetry_menu_items,
	.items_total=array_length(telemetry_menu_items)
};

void menu_structure_init(void)
{
	//setup name pointers
	for (uint8_t n = 0; n < IN_COUNT ; n++)
	{
		input_menu_items[n].name=input_names[n];
		output_inmix_items[n].name=input_names[n];
		input_menu_items[n].submenu=&input_properties_menu;
	}

	for (uint8_t n = 0; n < PPM_OUTPUT_COUNT ; n++)
	{
		output_menu_items[n].name=output_names[n];
		output_menu_items[n].submenu=&output_properties_menu;
		output_inmix_items[n].get_value=menu_output_inmix_getval;
		output_inmix_items[n].handler_func=menu_output_inmix_start;
	}
}
