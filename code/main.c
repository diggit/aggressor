#include <avr/io.h>
#include <string.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "config.h"
#ifdef STACK_MONITOR_ENABLED
#include "stack_monitor.h"
#endif

#include "analog.h"
#include "uart.h"
#include "ppm.h"
#include "storage.h"
#include "splash_image.h"
#include "3310_routines.h"
#include "hardware.h"
#include "systick.h"
#include "menu.h"
#include "homescreen.h"
#include "misc.h"
#include "output.h"

// #include "control.h"
// #include "bool.h"
// #include "input.h"

config_t config={
	.low_batt=108,
	.beep_keys=1,
	.beep=1,
	.beep_clock=1,
	.backlight=1
};

const char version_str[] PROGMEM={" version: "};
const char starting_str[] PROGMEM={" in starting..."};

int main (void)
{
	MCUCSR&=~(1<<WDRF);

	sei(); //enable interrupts

	#ifdef STACK_MONITOR_ENABLED
		stack_monitor_init();
	#endif

	uart_init();
	uart_puts(ID);
	uart_puts(pgmtoa(version_str));
	uart_num(_ver,1);
	uart_putc('.');
	uart_num(_subver,1);
	uart_puts(pgmtoa(starting_str));

	gpio_init();
	adc_init();
	beep_init();
	spi_init();
	LCD_init();

	output_load_defaults();

	storage_all_load();

	LCD_bitmap_pgm(image_data_aggressor_mod);
	LCD_update();
	delay_ms(1000);
	LCD_clear();

	// LCD_writeString_XY(10,2,itoa(sizeof(inputs[0].global),2),OVERWRITE); //8
	// LCD_writeString_XY(10,3,itoa(sizeof(inputs[0].model),2),OVERWRITE); //3
	// LCD_writeString_XY(10,4,itoa(sizeof(outputs[0]),2),OVERWRITE); //10
	// LCD_writeString_XY(10,5,itoa(EAD_MODEL_SLOT_SIZE,3),OVERWRITE); //82
	// LCD_update();
	// delay_ms(1000);
	// LCD_clear();

	menu_structure_init();


	homescreen_start();

	ppm_init();//start systick too

	while(1)
	{
		if(ppm_status.do_systick)
		{
			ppm_status.do_systick=0;
			systick();
		}
	}
}

// ISR(USART_RXC_vect)
// {
// 	uint8_t buff=UDR;
// 	if(buff=='R')//reset
// 	{
// 		Reset_AVR();//goodbye
// 	}
// 	else if(buff=='D')//dump
// 	{
// 		storage_dump_eeprom();
// 		beep(BEEP_NOTIF);
// 	}
// 	else if(buff=='E')//erase
// 	{
// 		storage_wipe_all();
// 		beep(BEEP_NOTIF);
// 	}
// 	else if(buff=='S')//save
// 	{
// 		storage_all_store();
// 		beep(BEEP_NOTIF);
// 	}
// }
