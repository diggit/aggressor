#ifndef _IDLE_LIB
	#define _IDLE_LIB

	#include <inttypes.h>

	#include "config.h"

	uint8_t	homescreen_input_bars_set(void);
	void	homescreen_input_bars(void);

	uint8_t	homescreen_telemetry_set(void);
	void	homescreen_telemetry(void);

	#define homescreen_next()	homescreen_switch(1)
	#define homescreen_prev()	homescreen_switch(-1)
	uint8_t homescreen_next_num(uint8_t new_homescreen_num);
	uint8_t homescreen_prev_num(uint8_t new_homescreen_num);
	void homescreen_switch(int8_t step);
	void homescreen_button_listener(uint8_t buttons);
	void homescreen_template_draw(void);
	void homescreen_draw(void);
	#define homescreen_start() homescreen_return()
	void homescreen_return(void);
#endif
