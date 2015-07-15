#include "stack_monitor.h"

#ifdef STACK_MONITOR_ENABLED

#include <avr/interrupt.h>

volatile uint16_t stack_monitor_max_size=0;
volatile uint16_t stack_monitor_last_size=0;

void stack_monitor_init(void)
{
	TIMSK|=(1<<TOIE0); //enable overflow interrupt
	TCNT0=0;
	TCCR0=(1<<CS02)|(1<<CS00); //prescale by 1024
	//OVF interrupt freq. is FOSC/(256*prescaler)
}

ISR(TIMER0_OVF_vect)
{
	stack_monitor_last_size=RAMEND-SP;
	if(stack_monitor_last_size>stack_monitor_max_size)
		stack_monitor_max_size=stack_monitor_last_size;
}
#endif
