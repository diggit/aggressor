#ifndef _SYSTICK_LIB
#define _SYSTICK_LIB

	#include <inttypes.h>

	#include "config.h"
	#include "screen.h"
	#include "3310_routines.h"

	//TODO: really should be menu draw funtion pointer declared here??

	#if ( defined LCD_MAX_FPS_DIVIDE && LCD_MAX_FPS_DIVIDE<=0 )
		#undef LCD_MAX_FPS_DIVIDE
	#endif

	void systick(void);
	void reset_oneps_loop(void);
	inline void set_new_drawer( void (*drawer)(void))
	{
		LCD_clear();
		screen_actual_draw_function=drawer;
		screen.redraw=1;
	}


#endif
