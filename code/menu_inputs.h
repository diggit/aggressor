#ifndef _MENU_INPUTS_LIB
#define _MENU_INPUTS_LIB

	#include <inttypes.h>

	#include "config.h"
	#include "input.h"

	void menu_input_notify_trim_change(input_p in,uint8_t in_num);

	void menu_input_trim_start(void);
	void menu_input_trim_draw(uint8_t val);
	char* menu_input_trim_getval(uint8_t unused);
	void menu_input_trim_flush(void);

	void menu_input_callibration_start(void);//guidepost for all input types calibration

	void menu_input_callibration_analog_start(void);
	void menu_input_callibration_analog_listener(uint8_t buttons);
	void menu_input_callibration_analog_draw(void);
	void menu_input_callibration_analog_draw_values(void);
	void menu_input_callibration_analog_draw_template(void);
	void menu_input_callibration_analog_draw__template(void);
	void menu_input_callibration_analog_draw_leveling_template(void);

	void menu_input_deadzone_start(void);
	char *menu_input_deadzone_getval(uint8_t unused);

	void menu_input_enabled_toggle(void);
	char *menu_input_enabled_getval(uint8_t unused);

	void menu_input_invert_toggle(void);
	char *menu_input_invert_getval(uint8_t unused);

	void menu_input_leveling_max_levels_start(void);
	void menu_input_leveling_max_levels_flush(void);
	char *menu_input_leveling_max_levels_getval(uint8_t unused);

	void menu_input_leveling_actual_level_start(void);
	char *menu_input_leveling_actual_level_getval(uint8_t unused);
	void menu_input_leveling_actual_level_draw(int8_t val);

	void menu_input_toggling_min_start(void);
	void menu_input_toggling_max_start(void);
	char *menu_input_toggling_max_getval(uint8_t unused);
	char *menu_input_toggling_min_getval(uint8_t unused);
	void menu_input_toggling_min_flush(void);
	void menu_input_toggling_max_flush(void);

#endif
