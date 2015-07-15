#ifndef _PPM_LIB
#define _PPM_LIB

	#include <avr/io.h>
	#include <inttypes.h>

	#include "config.h"

	#ifndef PPM_OUTPUT_COUNT
		#define PPM_OUTPUT_COUNT 3
	#endif

	#ifndef PPM_FREQUENCY
		#define PPM_FREQUENCY 50
	#endif

	// #define PPM_SET	bit_set(PORTD,(1<<5));
	// #define PPM_CLR	bit_clr(PORTD,(1<<5));
	#define PPM_PRESCALER	8 //must be supported by HW
	#define PPM_ONE_US		(FOSC/PPM_PRESCALER/1000000ul)
	#define POSITIVE_PULSE_LENGTH	350 //in us
	#define PPM_PULSE_DEFLECTION	500
	#define PPM_PULSE_CENTER	1500
	#define PPM_PULSE_EXTREME	200 //in us, how much could be value out of normal bounsa (mostly because of subtrims)
	#define PPM_PULSE_MIN		(PPM_PULSE_CENTER - PPM_PULSE_DEFLECTION)//in us
	#define PPM_PULSE_ABS_MIN	(PPM_PULSE_MIN - PPM_PULSE_EXTREME) //in us
	#define PPM_PULSE_MAX		(PPM_PULSE_CENTER + PPM_PULSE_DEFLECTION) //in us
	#define PPM_PULSE_ABS_MAX	(PPM_PULSE_MAX + PPM_PULSE_EXTREME) //in us
	#define PPM_TIMER_OVERFLOW	(FOSC/PPM_PRESCALER/PPM_FREQUENCY)
	#define PPM_SYSTICK_DELAY	500
	#define PPM_SYSTICK_MIN_DUR	7500//how much time of systick procedure must be done before next PPM cycle starts (us), eg LCD FB update could be done when pulses are generated,...
	#define PPM_SYSTICK			(PPM_OUTPUT_COUNT*PPM_PULSE_ABS_MAX+PPM_SYSTICK_DELAY)

	#if PPM_PRESCALER == 1
		#define PPM_TIMER_PRESCALER	(1<<CS10)
	#elif PPM_PRESCALER == 8
		#define PPM_TIMER_PRESCALER	(1<<CS11)
	#elif PPM_PRESCALER == 64
		#define PPM_TIMER_PRESCALER	((1<<CS11)|(1<<CS10))
	#elif PPM_PRESCALER == 256
		#define PPM_TIMER_PRESCALER	(1<<CS12)
	#elif PPM_PRESCALER == 1024
		#define PPM_TIMER_PRESCALER	((1<<CS12)|(1<<CS10))
	#else
		#error "No such prescaler available!"
	#endif

	#if PPM_ONE_US < 1
		#error "minimal 1us precission is not possible with this prescaler!"
	#endif


	#if PPM_TIMER_OVERFLOW > 65535
		#error "PPM frequency above wanted value, try increasing prescaler!"
	#endif

	#if PPM_SYSTICK*PPM_ONE_US + PPM_SYSTICK_MIN_DUR*PPM_ONE_US > PPM_TIMER_OVERFLOW
		#error "no time for systick!"
	#endif

	extern volatile uint16_t ppm_output[];

	volatile typedef struct{
		uint8_t do_systick:1;
		uint8_t enabled:1;
	}ppm_status_t;

	extern volatile ppm_status_t ppm_status;


	void ppm_init(void);
	// ISR(TIMER1_COMPA_vect);
	// ISR(TIMER1_COMPB_vect);

#endif
