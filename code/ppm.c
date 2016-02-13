#include "ppm.h"
#include "misc.h"
#include <avr/interrupt.h>
#include "ppm.h"

volatile uint16_t ppm_output[PPM_OUTPUT_COUNT]={1500*PPM_ONE_US,1500*PPM_ONE_US,1500*PPM_ONE_US,1500*PPM_ONE_US,1500*PPM_ONE_US};//some defaults
volatile uint8_t ppm_pulse;//which pulse is generated

volatile ppm_status_t ppm_status={0,1};

//TIMER0 - 8bit  - systick
//TIMER1 - 16bit - ppm
//

void ppm_init()
{
	TCNT1=0;//clear counter
	bit_set(TIMSK,(1<<OCIE1A)|(1<<OCIE1B));//both compare interrupt enable
	OCR1A=0;
	OCR1B=PPM_SYSTICK*PPM_ONE_US;
	// OCR1A=OCR1A+POSITIVE_PULSE_LENGTH*PPM_ONE_US;
	//TCCR1B=(1<<WGM12)|(1<<WGM13);//count to ICR1 value
	// PPM_SET;
	TCCR1A=(1<<COM1A0)|(1<<COM1A1);//next interrupt will SET OC1A
	TCCR1B=(1<<WGM12)|(1<<WGM13)|PPM_TIMER_PRESCALER;//set mode to CTC to ICR1 (count to value in ICR1)
	ICR1=PPM_TIMER_OVERFLOW;//to set correct frequency

}

#define EDGE_RISING		1
#define EDGE_FALLING	2
volatile uint8_t edge=EDGE_RISING;


//PPM pulses
ISR(TIMER1_COMPA_vect)
{
	if(edge==EDGE_RISING)
	{
		TCCR1A=(1<<COM1A1);//next COMPA interrupt will CLEAR OC1A
		edge=EDGE_FALLING;
		OCR1A+= POSITIVE_PULSE_LENGTH*PPM_ONE_US;//clear PPM after constant time after PPM is set
	}
	else
	{
		if(ppm_status.enabled)
			TCCR1A=(1<<COM1A0)|(1<<COM1A0);//next COMPA interrupt will SET OC1A
		edge=EDGE_RISING;
		if(ppm_pulse>array_length(ppm_output)-1) //don't ask me why -1 here, It just works...
		{
			ppm_pulse=0;
			OCR1A=0;
		}
		else//common falling edge
		{
			OCR1A+= ppm_output[ppm_pulse] - POSITIVE_PULSE_LENGTH*PPM_ONE_US;//set time, when should be next rising edge
			ppm_pulse++;
		}
	}
}

ISR(TIMER1_COMPB_vect)
{
	ppm_status.do_systick=1;
}
