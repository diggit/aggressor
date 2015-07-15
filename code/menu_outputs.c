#include "menu_outputs.h"
#include "output.h"
#include "menu.h"
#include "3310_routines.h"

//SUBTRIM value functions
void menu_output_subtrim_start()
{
	menu_simple_lister_signed_start(&outputs[menu->parent->hilighted].subtrim_val, OUTPUT_SUBTRIM_MIN , OUTPUT_SUBTRIM_MAX, menu_output_subtrim_draw);
}

void menu_output_subtrim_draw(int8_t val)
{
	*lister_original_value_p.s=val;//immediately use value
	menu_items_draw_value(MENU_ACTIVE_LINE,itoa(val,MENU_NUMBER_LENGTH),OVERWRITE|INVERT);
}

char* menu_output_subtrim_getval(uint8_t unused)
{
	return itoa(outputs[menu->parent->hilighted].subtrim_val,MENU_NUMBER_LENGTH);
}

//inMIX
void menu_output_inmix_start()
{
	menu_simple_lister_signed_start(&outputs[menu->parent->parent->hilighted].in_coef[menu->hilighted],-10, 10, NULL);
}

// void menu_output_inmix_draw(int8_t val)
// {
// 	menu_items_draw_value(MENU_ACTIVE_LINE,itoa(val,MENU_NUMBER_LENGTH),OVERWRITE|INVERT);
// }

char* menu_output_inmix_getval(uint8_t val)
{
	return itoa(outputs[menu->parent->parent->hilighted].in_coef[val],MENU_NUMBER_LENGTH);
}




//EPA SETTINGS

//universal draw with immediate value flush
void menu_output_epa_draw(int8_t val)
{
	*lister_original_value_p.s=val;
	menu_items_draw_value(MENU_ACTIVE_LINE,itoa(val,MENU_NUMBER_LENGTH),OVERWRITE|INVERT);
}

void menu_output_epa_min_start(void)
{
	menu_simple_lister_signed_start(&outputs[menu->parent->hilighted].epa_min,OUTPUT_EPA_MIN, 0 , menu_output_epa_draw); //outputs[menu->parent->hilighted].epa_max-1
	//TODO: allow positive value for epa_min and vice versa
}

char *menu_output_epa_min_getval(uint8_t unused)
{
	return itoa(outputs[menu->parent->hilighted].epa_min,MENU_NUMBER_LENGTH);
}


void menu_output_epa_max_start(void)
{
	menu_simple_lister_signed_start(&outputs[menu->parent->hilighted].epa_max, 0 , OUTPUT_EPA_MAX , menu_output_epa_draw); //outputs[menu->parent->hilighted].epa_max-1
	//TODO: allow positive value for epa_max and vice versa
}

char *menu_output_epa_max_getval(uint8_t unused)
{
	return itoa(outputs[menu->parent->hilighted].epa_max,MENU_NUMBER_LENGTH);
}
