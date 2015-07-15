#ifndef __MISC_LIB
	#define __MISC_LIB

	#include <inttypes.h>
	#include <avr/wdt.h>
	#include <avr/pgmspace.h>
	#include <avr/eeprom.h>

	#define uabs(x)  ( ( (x) < 0) ? -(x) : (x) )

	//global macros for bitwise manipulation
	#define bit_get(p,m) ((p) & (m))
	#define bit_set(p,m) ((p) |= (m))
	#define bit_clr(p,m) ((p) &= ~(m))
	#define bit_flip(p,m) ((p) ^= (m))
	#define bit_con(c,p,m) (c ? bit_set(p,m) : bit_clr(p,m)) //macro for conditional bit set/clr
	#define BIT(x) (0x01 << (x))
	#define LONGBIT(x) ((unsigned long)0x00000001 << (x))

	//this is epic idea... http://stackoverflow.com/a/7919546
	#define FILL_BITS_FROM_LSB(bits) ((1<<(bits))-1)
	#define FILL_BITS_FROM_MSB(bits) (~((0x80>>((bits)-1))-1)) //meh, you get it?

	//simple NOP, belongs to empty loops etc.
	#define NOP __asm__("NOP");
	#define Reset_AVR() wdt_enable(WDTO_30MS); while(1) {NOP;}//little workaround, atmega has no software reset

	#define array_length(A) sizeof(A)/sizeof(A[0])

	#define NULL ((void *)0)

	#define BUFF_SIZE	32

	uint32_t	reduce(uint32_t value, uint8_t decrease);
	char* etoa(uint16_t ee_address, uint8_t bytes);
	char* stoa(const char *str);
	char* ctoa(char c);
	char* pgmtoa(const char *pgm_string);
	char* itoa_dec_fill(int32_t val,int min,char filler,int decimals);
	char* itoa_fill(int32_t val, int min, char filler);
	char* itoa(int32_t val, int min);
	char* itoa_dec(int32_t val,int min,int decimals);
	void  delay_us(uint32_t delay);
	void  delay_ms(uint16_t delay);

	inline uint16_t crop(uint16_t val,  uint16_t min, uint16_t max)
	{
		if(val<min)
			return min;
		if(val>max)
			return max;
		return val;
	}




#endif
