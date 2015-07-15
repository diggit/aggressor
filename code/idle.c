#include "idle.h"
#include "3310_routines.h"
#include "screen.h"
#include "menu.h"
#include "control.h"
#include "clock.h"
#include "menu_model.h"
#include "input.h"
#include "systick.h"


void idle_button_listener(uint8_t buttons)
{
	switch (buttons) {
		case BTN_FUNC_LONG:
			menu_start();
			break;
		case BTN_SEL:
			clock_toggle();
			break;
		case BTN_SEL_LONG:
			clock_reset();
			break;
	}
}

void idle_template_draw()
{
	LCD_write_header_text(model_name);
}


#define BAR_VERTICAL_TOP
void idle_variables_draw()
{
	LCD_drawBar(10,40,64,8 ,(int32_t)(IN_NORM+inputs[IN_STEER].runtime.value)/(2*IN_NORM/100),BAR_HORIZONTAL);//bottom horizontal bar
	LCD_drawBar(0 ,22,8 ,26,-(int32_t)(IN_NORM+inputs[IN_THROTTLE].runtime.value)/(2*IN_NORM/100),BAR_VERTICAL);//left verical bar
	LCD_drawBar(76,22,8 ,26,-(int32_t)(IN_NORM+inputs[IN_DR].runtime.value)/(2*IN_NORM/100),BAR_VERTICAL);//right vertical bar
	LCD_drawBar(0 ,12,20,8 ,(int32_t)(IN_NORM+inputs[IN_THUMB].runtime.value)/(2*IN_NORM/100),BAR_HORIZONTAL);//left horizontal bar
	LCD_drawBar(HW_COLUMNS-20,12,20,8,(int32_t)(IN_NORM+inputs[IN_ALT].runtime.value)/(2*IN_NORM/100),BAR_HORIZONTAL);//right horizontal bar


	// LCD_schedule_update(ALL_ROWS_MODIFIED);
	// LCD_writeString_XY(10,2,itoa(inputs[IN_DR].value,6),OVERWRITE);
	// LCD_writeString_XY(10,3,itoa(inputs[IN_ALT].level_actual,5),OVERWRITE);
	// LCD_writeString_XY(10,4,itoa(inputs[IN_ALT].levels[inputs[IN_ALT].level_actual],5),OVERWRITE);
	//LCD_writeString_XY(10,4,itoa(adc_measured[ADC_ALT],4),OVERWRITE);
	// LCD_writeString_XY(10,2,itoa(adc_measured[ADC_TRIM_TOP],4),OVERWRITE);
	// LCD_writeString_XY(10,4,itoa(adc_measured[ADC_TRIM_RIGHT],4),OVERWRITE);

	clock_draw_cond();
}

void idle_draw()
{
	//LCD_clear();
	screen_template_draw();
	idle_template_draw();
	clock_draw();
	//LCD_writeString_XY(10,3,"something...",0);
	//LCD_writeString_XY(10,4,"something...",INVERT);

}

void idle_start()
{
	menu_state=MENU_STATE_IDLE;
	fiftyps_periodical_call=idle_variables_draw;
	//oneps_periodical_call=clock_draw;
	set_event_listener(idle_button_listener);
	set_new_drawer(idle_draw);
}
