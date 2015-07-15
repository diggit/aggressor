#ifndef _OUTPUT_LIB
	#define _OUTPUT_LIB

	#include <inttypes.h>
	#include <stdint.h> //for max value of int32_t

	#include "config.h"
	#include "input.h"
	#include "ppm.h"

	#define OUTPUT_CH1	0
	#define OUTPUT_CH2	1
	#define OUTPUT_CH3	2
	#define OUTPUT_CH4	3
	#define OUTPUT_CH5	4

	#define OUTPUT_EPA_MAX	100
	#define OUTPUT_EPA_MIN	(-OUTPUT_EPA_MAX)

	#define OUTPUT_SUBTRIM_MAX	100
	#define OUTPUT_SUBTRIM_MIN	(-OUTPUT_SUBTRIM_MAX)

	#define OUTPUT_INMIX_COEF_MAX	10
	#define OUTPUT_INMIX_COEF_MIN	(-OUTPUT_INMIX_COEF_MAX)

	#ifndef OUTPUT_SUBTRIM_USER_MULTIPLIER
		#define OUTPUT_SUBTRIM_USER_MULTIPLIER 1
	#endif
	#define OUTPUT_SUBTRIM_MULTIPLIER	(IN_NORM/(PPM_PULSE_DEFLECTION)*PPM_ONE_US*OUTPUT_SUBTRIM_USER_MULTIPLIER)

	#if (PPM_PULSE_DEFLECTION * PPM_ONE_US)* (IN_NORM*OUTPUT_INMIX_COEF_MAX*HW_INPUTS) / (OUTPUT_INMIX_COEF_MAX*HW_INPUTS) * OUTPUT_EPA_MAX >= INT32_MAX
		#error "possible overflow detected! try decreasing IN_NORM, you max try to edit formula in output.c 'out->value =...'"
	#endif

	#if (PPM_PULSE_DEFLECTION * PPM_ONE_US)* (IN_NORM*OUTPUT_INMIX_COEF_MAX*HW_INPUTS) >= INT32_MAX
		#error "possible overflow detected! try decreasing OUTPUT_INMIX_COEF_MAX or IN_NORM, you max try to edit formula in output.c 'out->value =...'"
	#endif

	typedef struct{
		int8_t	subtrim_val;
		int8_t	epa_min;
		int8_t	epa_max;
		uint16_t	value;
		int8_t	in_coef[HW_INPUTS];
	}output_t;

	typedef output_t *output_p;

	extern output_t outputs[PPM_OUTPUT_COUNT];
	extern const char *output_names[PPM_OUTPUT_COUNT];

	void output_eval_all(void);
	void output_push_to_ppm(void);
	void output_load_defaults(void);

#endif
