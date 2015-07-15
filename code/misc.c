#include "misc.h"

uint32_t reduce(uint32_t value, uint8_t decrease) //correct rounding, not just cropping
{
	uint16_t dec=1;
	for(;decrease>1;decrease--)//calculate divider
		dec*=10;

	if (value/dec%10 > 4)//round
		return value/(10*dec)+1;
	else
		return value/(10*dec);
}

char char_buffer[BUFF_SIZE] = {0};

char* etoa(uint16_t ee_address, uint8_t bytes)
{
	uint8_t length=0;
	char c=eeprom_read_byte((uint8_t*)ee_address++);

	while(c && length<BUFF_SIZE-1 && bytes--)
	{
		char_buffer[length]=c;
		length++;
		c=eeprom_read_byte((uint8_t*)ee_address++);
	}
	char_buffer[length]='\0';
	return char_buffer;
}

char* stoa(const char *str)
{
	uint8_t length;
	for(length=0;str[length]!='\0' && length<BUFF_SIZE-1;length++)
		char_buffer[length]=str[length];
	char_buffer[length]='\0';
	return char_buffer;
}

char* ctoa(char c)
{
	char_buffer[0]=c;
	char_buffer[1]='\0';
	return char_buffer;
}

char* pgmtoa(const char *pgm_string)
{
	uint8_t length=0;
	char c=pgm_read_byte(pgm_string++);

	while(c && length<BUFF_SIZE-1)
	{
		char_buffer[length]=c;
		length++;
		c=pgm_read_byte(pgm_string++);
	}
	char_buffer[length]='\0';
	return char_buffer;
}

char* itoa_dec_fill(int32_t val,int min,char filler,int decimals)//coverts number to string with defined base and minimal digit, char fillers
{

	uint8_t i = 30; //max characters
	uint8_t negative=0;

	if (val==0)
	{
		char_buffer[i--]='0';
	}

	else
	{
		if(val<0)
		{
			negative=1;
			val=-val;
		}
		for (; (val && i); --i, val /= 10)
		{
			if(30-i==decimals && decimals!=0 )
				char_buffer[i--]='.';

			char_buffer[i] = "0123456789abcdef"[val % 10];
		}

	}

	if(30-i<=decimals)
	{
		for(;30-i<decimals;)
			char_buffer[i--]='0';
		char_buffer[i--]='.';
		char_buffer[i--]='0';
	}

	if(negative)
		char_buffer[i--]='-';

	for (; min+i>30; --i)
		char_buffer[i] = filler;

	return &char_buffer[i+1];
}

char* itoa_fill(int32_t val, int min, char filler)
{
	return itoa_dec_fill(val,min,filler,0);
}

char* itoa(int32_t val, int min)
{
	return itoa_dec_fill(val,min,' ',0);
}

char* itoa_dec(int32_t val,int min,int decimals)//coverts number to string with defined base and minimal digit, char fillers
{
	return itoa_dec_fill(val,min,' ',decimals);
}

void delay_us(uint32_t delay)//takes 8 cycles, at 16MHz means half of micro second (freq*time)/repeated
{
	delay=delay<<1;//multiply by 2 to get 1uS delay
	//we are running 8MHz, for now...
	while(delay--) {NOP;NOP;}
}//dont try to optimize, it's calibrated! (gcc -O2)

void delay_ms(uint16_t delay) //calibrated using oscope
{
	while(delay--) delay_us(900);
}
