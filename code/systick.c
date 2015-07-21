#include "systick.h"
#ifdef STACK_MONITOR_ENABLED
	#include "stack_monitor.h"
#endif
#include "control.h"
#include "analog.h"
#include "3310_routines.h"
#include "input.h"
#include "hardware.h"
#include "screen.h"
#include "output.h"
#include "clock.h"
#include "trim.h"

uint8_t loop=ONE_SECOND_TICK_CALIBRATION;

#ifdef LCD_MAX_FPS_DIVIDE
uint8_t fps_divider=0;
#endif

void systick()
{
	beep_stop_cond();
	adc_measure_all();
	input_eval_all();
	trim_read_all();
	output_eval_all();
	output_push_to_ppm();
	//until here, everything must be done earlier that next PPM cycle occur

	//foloowing is time relaxed...
	beep_start();

	loop+=1;

	if (loop>=ONE_SECOND_TICK_CALIBRATION) //divide by N, for calibrated 1 second
	{
		//processed once per second
		loop=0;
		//stack_test(0);
	}


	//here starts UI handling
	control_eval_buttons();//calls listener if button pressed (and if listener is set)

	if(screen.redraw > 1)
	{
		screen.redraw--;
	}
	else if(screen.redraw == 1)
	{
		//beep(1000,200);
		screen_redraw(); //calls screen drawer if set
		//clears screen and then calls se drawer so any additional drawing should happen later
	}


	//thins that should happen every second
	if(loop==0)
	{
		screen_battery_draw(0);
		if(adc_battery_voltage<config.low_batt && battery_trigger<BATTERY_TRIGGER_DEGLITCH)//when low voltage measured and triggert deglitch not reached
		{
			battery_trigger++;
		}
		else if(battery_trigger<BATTERY_TRIGGER_DEGLITCH)//when voltage OK, and trigger have not reached deglitch value yet
		{
			battery_trigger=0;
		}
	}
	if(battery_trigger>=BATTERY_TRIGGER_DEGLITCH)
	{
		//this is LOW batt tune
		if(loop==ONE_SECOND_TICK_CALIBRATION/10)
			beep(2000,60);
		else if(loop==2*ONE_SECOND_TICK_CALIBRATION/10)
			beep(1000,60);
		else if(loop==3*ONE_SECOND_TICK_CALIBRATION/10)
			beep(2000,150);

		if(loop==ONE_SECOND_TICK_CALIBRATION/2)
			screen_battery_draw(INVERT);
	}

	//call clock to handle tick (or not)
	clock_count();

	//periodical call functions
	if(fiftyps_periodical_call!=NULL)
		fiftyps_periodical_call();


	if(loop==0 && oneps_periodical_call!=NULL)
	{
		oneps_periodical_call();
	}

	//notify when error drawing outside of FB
	if(LCD_error.draw_outside)
	{
		LCD_writeString_XY(40,0,"E",OVERWRITE);
		LCD_error.draw_outside=0;
	}



	//push FB contents to LCD (may be divided), LCD_update decides if anything was updated and FB push to LCD is really necessary
	#ifdef LCD_MAX_FPS_DIVIDE //in case of divider > 1
		fps_divider++;
		if(fps_divider==LCD_MAX_FPS_DIVIDE)
		{
			fps_divider=0;
			LCD_update(); //real updates occur when something modified lcd_buffer
		}
	#else
		LCD_update(); //real updates occur when something modified lcd_buffer
	#endif

	#ifdef STACK_MONITOR_ENABLED
	uart_puts(itoa(stack_monitor_last_size,4));
	uart_putc('<');
	uart_puts(itoa(stack_monitor_max_size,4));
	uart_putc('\n');
	//return;
	#endif


	//chck if power button if in OFF position...
	if(!(PINC&(1<<2)))//if power lost
		run_poweroff_sequence();




}

void reset_oneps_loop()
{
	loop=0;
}
