#ifndef _STACK_MONITOR_LIB
	#define _STACK_MONITOR_LIB
	#include "config.h"

	#ifdef STACK_MONITOR_ENABLED

	#include <inttypes.h>


	extern volatile uint16_t stack_monitor_max_size;
	extern volatile uint16_t stack_monitor_last_size;

	void stack_monitor_init(void);
	// ISR(TIMER0_COMP_vect);
	#else
		#define stack_monitor_max_size	0
	#endif

#endif
