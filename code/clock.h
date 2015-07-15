#ifndef _CLOCK_LIB
	#define _CLOCK_LIB

	#include <inttypes.h>

	#include "config.h"
	#include "bool.h"

	extern uint32_t timer;
	extern bool timer_counting;

	#ifndef CLOCK_PRESCALE
		#define CLOCK_PRESCALE 50
	#endif

	#define CLOCK_NOTIF_X 50

	void clock_start(void);
	void clock_stop(void);
	void clock_toggle(void);
	void clock_reset(void);
	void clock_count(void);
	void clock_draw_cond(void);
	void clock_draw(void);

#endif
