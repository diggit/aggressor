#ifndef _MENU_SIMPLE_CONTROLS_LIB
	#define _MENU_SIMPLE_CONTROLS_LIB

	#include <inttypes.h>

	#include "config.h"

	void menu_beep_toggle(void);
	char* menu_beep_get_val(uint8_t unused);

	void menu_beep_keys_toggle(void);
	char* menu_beep_keys_get_val(uint8_t unused);

	void menu_ppm_toggle(void);
	char* menu_ppm_get_val(uint8_t unused);

	void menu_lowbatt_start(void);
	void menu_lowbatt_draw(uint8_t val);
	char* menu_lowbatt_getval(uint8_t unused);
	void menu_lowbatt_flush(void);

#endif
