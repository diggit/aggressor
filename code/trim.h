#ifndef __trim
#define __trim

#include	<inttypes.h>

#include "config.h"

#define TRIM_NONE	0
#define TRIM_LEFT  1
#define TRIM_TOP   2
#define TRIM_RIGHT 3

#define TRIM_COUNT 4

//which bit indicated what...

#define TRIM_LEFT_INC (TRIM_LEFT*2)
#define TRIM_LEFT_DEC (TRIM_LEFT*2+1)

#define TRIM_TOP_INC (TRIM_TOP*2)
#define TRIM_TOP_DEC (TRIM_TOP*2+1)

#define TRIM_RIGHT_INC (TRIM_RIGHT*2)
#define TRIM_RIGHT_DEC (TRIM_RIGHT*2+1)

#ifndef TRIM_MULTIPLIER
	#define TRIM_MULTIPLIER 1
#endif

typedef struct{
	uint8_t inc:1;
	uint8_t dec:1;
	const char *name;
} trim_t;

#define TRIM_MAX	100
#define TRIM_MIN	-TRIM_MAX

extern trim_t trims[TRIM_COUNT];

void trim_read_all(void);
void trim_get(uint8_t);








#endif
