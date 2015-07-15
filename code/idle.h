#ifndef _IDLE_LIB
	#define _IDLE_LIB

	#include <inttypes.h>

	#include "config.h"

	void idle_button_listener(uint8_t buttons);
	void idle_template_draw(void);
	void idle_variables_draw(void);
	void idle_draw(void);
	void idle_start(void);
#endif
