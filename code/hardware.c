#include "hardware.h"
#include "uart.h"
#include "storage.h"
#include "misc.h"

uint8_t battery_trigger;

void gpio_init()
{

//PORTA - FULL ADC USE
	// DDRA=0;
	// PORTA=0;

//PORTB
	//PIN 0 IN	THUMB BTN	ext pull
	//PIN 1	IN	SET BTN		ext pull
	//PIN 2	IN	FUNC BTN	ext pull
	//PIN 3	IN	DEC BTN		ext pull
	//PIN 4 OUT	BACKLIGHT		MOD, was INC BTN
	//PIN 5 OUT	MOSI to display
	//PIN 6 IN  MISO from display
	//PIN 7 OUT	CLK to display
	DDRB=(1<<5)|(1<<7)|(1<<4);
	PORTB=(1<<4);
//PORTC
	//PIN 0 NC
	//PIN 1 NC
	//PIN 2 IN power present
	//PIN 3 OUT disable power
	//PIN 4 NC
	//PIN 5 NC
	//PIN 6 IN INC BTN		MOD, was ...
	//PIN 7 ?? DISP5
	DDRC=(1<<3);
	PORTC=0;
//PORTD
	//PIN 0 RX (PULL)
	//PIN 1 TX
	//PIN 2 NC
	//PIN 3 OUT D/C (DISP1)
	//PIN 4 OUT RST (DISP2)
	//PIN 5 OUT	PPM
	//PIN 6 OUT SCE(DISP4)
	//PIN 7 OUT	BUZZER
	DDRD=(1<<5)|(1<<7)|(1<<3)|(1<<4)|(1<<6);
	PORTD=(1<<0);
}

volatile beep_setup_t scheduled_beep;

void beep_init()
{
	TCCR2=(1<<WGM21)|(1<<COM20);
	//WGM21 -> CTC
	//COM20 -> OC2 toggle on cmp match
}

#define STOP_BEEP_COUNTER() bit_clr(TCCR2,(1<<CS22)|(1<<CS21)|(1<<CS20))
#define START_BEEP_COUNTER() bit_set(TCCR2,(1<<CS22))//32 prescaler

#define BEEP_PHASE_STOP		0
#define BEEP_PHASE_START	1
#define BEEP_PHASE_BEEP		2
#define BEEP_PHASE_PAUSE	3
uint8_t beep_phase=0;
uint16_t beep_duration=0;

void beep_process()
{
	if(beep_phase==BEEP_PHASE_STOP)
		return;

	else if(beep_phase==BEEP_PHASE_START)//first call, we have setup for beep, but not yet running
	{
		OCR2=(uint32_t)FOSC/((uint32_t)2*scheduled_beep.frequency*32)-1;//set frequency to HW timer
		TCNT2=0;//clear counter
		beep_phase=BEEP_PHASE_BEEP;
		START_BEEP_COUNTER();
		beep_duration=0;
	}
	else
	{
		if(scheduled_beep.duration_ms>beep_duration)//phase not yet finished, mabe next cycle...
		{
			beep_duration+=BEEP_TIME_STEP;
		}
		else//end of some phase
		{
			beep_duration=0;
			if(beep_phase==BEEP_PHASE_BEEP)//end of beep phase
			{
				STOP_BEEP_COUNTER();//stop beeping
				TCNT2=0;//clear counter
				if(scheduled_beep.repeats>0)//if there are more repeats
				{
					beep_phase=BEEP_PHASE_PAUSE;
				}
				else//this is end of beep(sequence)
					beep_phase=BEEP_PHASE_STOP;
			}
			else if(beep_phase==BEEP_PHASE_PAUSE)//end of pause phase
			{
				beep_phase=BEEP_PHASE_BEEP;//after pahuse, there is always beep
				START_BEEP_COUNTER();
				scheduled_beep.repeats--;
			}
		}
	}
}


void beep(uint16_t frequency, uint16_t duration_ms, uint16_t pause_ms,uint8_t repeats)
{
	if (!(config.beep))//ignore beep calls when beep disabled
		return;
	if(beep_phase!=BEEP_PHASE_STOP)//terminate runing beep
		STOP_BEEP_COUNTER();
	scheduled_beep.frequency=crop(frequency,BEEP_FREQ_MIN,BEEP_FREQ_MAX);;
	scheduled_beep.duration_ms=duration_ms;
	scheduled_beep.pause_ms=pause_ms;
	scheduled_beep.repeats=repeats;
	beep_phase=BEEP_PHASE_START;

}

const char goodbye_str[] PROGMEM={"goodbye..."};
void force_poweroff()
{
	//TODO: add capacitor to base of shutdown transistor (patallel to R18)
	uart_puts(pgmtoa(goodbye_str));
	PORTC|=(1<<3);
}

const char power_lost_str[] PROGMEM={"switch OFF!\n"};
const char eeprom_write_done_str_str[] PROGMEM={"EWD\n"};

void run_poweroff_sequence()
{
	uart_puts(pgmtoa(power_lost_str));
	storage_all_store();
	uart_puts(pgmtoa(eeprom_write_done_str_str));
	// PORTB&=~(1<<4);//off backlight
	//delay_ms(100);
	force_poweroff();
}
