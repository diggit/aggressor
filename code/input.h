#ifndef _INPUT_LIB
#define _INPUT_LIB

#include <inttypes.h>

#include "config.h"

#define IN_STEER	0
#define IN_THROTTLE 1
#define IN_DR		2
#define IN_ALT		3
#define IN_THUMB	4

#define IN_TYPE_ANALOG		1
#define IN_TYPE_TOGGLING	2
#define IN_TYPE_LEVELING	3

#define IN_LEVELING_MAX_LEVELS		5

#define IN_DIGITAL_PINREG	PINB
#define IN_DIGITAL_MAX		100
#define IN_DIGITAL_MIN		(-IN_DIGITAL_MAX)


#define IN_ANALOG_MAX		1023
#define IN_ANALOG_MIN		0

#ifndef IN_NORM
	#define  IN_NORM 1000
#endif

#if !defined(IN_COUNT) ||  IN_COUNT<=0
	#error "IN_COUNT must be greather than 1"
#endif


#ifndef IN_TRIM_USER_MULTIPLIER
	#define IN_TRIM_USER_MULTIPLIER 1
#endif
#define INPUT_TRIM_MULTIPLIER (IN_NORM/(PPM_PULSE_DEFLECTION)*PPM_ONE_US*IN_TRIM_USER_MULTIPLIER)//multiply by N to align with 1uS resolution


typedef struct{
	union
	{
		struct
		{
			uint8_t adc_channel;
			int16_t raw_min;
			int16_t raw_max;
			int16_t raw_center; // input saves here previous state of button to detect rising edge
			uint8_t enabled:1;
			uint8_t invert:1;
			uint8_t unused_0:2;//place for type, access it through .any
		}analog;
		struct
		{
			uint8_t pin;
			int16_t min;
			int16_t max;
			int16_t edge_detect; // input saves here previous state of button to detect rising edge
			uint8_t enabled:1;
			uint8_t invert:1;
			uint8_t unused_0:2;//place for type, access it through .any
		}digital;
		struct
		{
			uint8_t unused_0;
			int16_t unused_1;
			int16_t unused_2;
			int16_t unused_3; // input saves here previous state
			uint8_t enabled:1;
			uint8_t invert:1;
			uint8_t type:2;//well, this should be outside of any struct, but then it would occupy whole byte
			uint8_t unused_4:4; //just as aligner
		}common;
	}global;//same for all models


	struct
	{
		uint16_t raw_value;
		int16_t value;
		int8_t *levels; //pointer to array of levels, pointer should not be saved
	}runtime;//only for runtime, don't save

	union {
		struct{
			uint8_t deadzone; //for analog type
			int8_t	trim_val;
			uint8_t	trim:4;
		}analog;
		struct{
			uint8_t level_count;//for leveling type
			uint8_t level_actual:4;
		}digital;
	}model; //model specific
}input;

typedef input *input_p;

extern int8_t in_alt_levels[IN_LEVELING_MAX_LEVELS];

extern input inputs[IN_COUNT];
extern const char *input_names [IN_COUNT];

void input_eval_all(void);

#endif
