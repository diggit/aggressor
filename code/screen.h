#ifndef _SCREEN_LIB
#define _SCREEN_LIB

	#include <inttypes.h>

	#include "config.h"

	#define SCREEN_REDRAW_1S	50
	#define SCREEN_REDRAW_100MS	5

	extern void (*screen_actual_draw_function)(void);

	typedef struct{
		uint8_t redraw;//drawing functions will be called, whole screen redrawn
		uint8_t update:1;//push all buffer contets to LCD
	}screen_t ;

	extern volatile screen_t screen;

	void screen_schedule_redraw(uint8_t delay_ds);
	void screen_redraw(void);
	void screen_template_draw(void);
	void screen_battery_draw(uint8_t addflag);

#endif
