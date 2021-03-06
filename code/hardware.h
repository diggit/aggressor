#ifndef _HARDWARE_LIB
#define _HARDWARE_LIB
	#include <avr/io.h>

	#include "config.h"
	#include "ppm.h"


	#define BEEP_TIME_STEP (1000/PPM_FREQUENCY)

	#define BEEP_FREQ_MIN	150//(FOSC/(2*32*256))

	#ifndef BEEP_FREQ_MAX
		#define BEEP_FREQ_MAX 5000//(FOSC/(2*32))
	#endif

	typedef struct{
		uint16_t frequency;
		uint16_t duration_ms;
		uint16_t pause_ms;
		uint8_t repeats;
	} beep_setup_t;

	volatile beep_setup_t scheduled_beep;

	extern uint8_t battery_trigger;

	void gpio_init(void);
	void beep_init(void);
	void beep_process(void);
	void beep(uint16_t frequency, uint16_t duration_ms, uint16_t pause_ms,uint8_t repeats);
	void force_poweroff(void);
	void run_poweroff_sequence(void);


#endif
