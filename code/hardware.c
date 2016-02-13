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


void beep_start()
{
	if(scheduled_beep.duration_ms==0 || scheduled_beep.frequency==0)//if not valid settings
		return;

	if( TCCR2 & ((1<<CS22)|(1<<CS21)|(1<<CS20)) )//if any clock input to counter ->already running
		return;

	TCNT2=0;//clear counter

	scheduled_beep.frequency=crop(scheduled_beep.frequency,BEEP_FREQ_MIN,BEEP_FREQ_MAX);
	//prescaler 32 is ok for frequency upto 2kHz, higher freqs. has too low resolution
	OCR2=(uint32_t)FOSC/((uint32_t)2*scheduled_beep.frequency*32)-1;
	TCCR2= (TCCR2 & ~((1<<CS22)|(1<<CS21)|(1<<CS20))) | (1<<CS22);//32 prescaler
}

void beep_stop_cond()
{

	if( !( TCCR2 & ((1<<CS22)|(1<<CS21)|(1<<CS20)) ))//if no clock input to counter
		return;

	if(scheduled_beep.duration_ms>=BEEP_TIME_STEP)
	{
		scheduled_beep.duration_ms-=BEEP_TIME_STEP;
	}
	else
	{
		bit_clr(TCCR2,(1<<CS22)|(1<<CS21)|(1<<CS20));
		scheduled_beep.duration_ms=0;
		scheduled_beep.frequency=0;
	}
}

void beep(uint16_t frequency, uint16_t duration_ms)
{
	if (!(config.beep))//ignore beep calls when beep disabled
		return;
	scheduled_beep.frequency=frequency;
	scheduled_beep.duration_ms=duration_ms;
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
