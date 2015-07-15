#ifndef _MENU_OUTPUTS_LIB
	#define _MENU_OUTPUTS_LIB

	#include <inttypes.h>

	#include "config.h"

	void menu_output_subtrim_start(void);
	void menu_output_subtrim_draw(int8_t val);
	char* menu_output_subtrim_getval(uint8_t val);

	void menu_output_inmix_start(void);
	//void menu_output_inmix_draw(int8_t val);
	char* menu_output_inmix_getval(uint8_t unused);

	void menu_output_epa_draw(int8_t val);

	void menu_output_epa_min_start(void);
	char *menu_output_epa_min_getval(uint8_t unused);

	void menu_output_epa_max_start(void);
	char *menu_output_epa_max_getval(uint8_t unused);

#endif
