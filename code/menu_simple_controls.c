#include "menu_simple_controls.h"
#include "menu.h"
#include "ppm.h"
#include "misc.h"
#include "3310_routines.h"

void menu_beep_toggle(void)
{
	config.beep ^= 1;
	menu_return();
}

char* menu_beep_get_val(uint8_t unused)
{
	return pgmtoa(config.beep ? messages[ON_STR]:messages[OFF_STR]);
}


void menu_beep_keys_toggle(void)
{
	config.beep_keys ^= 1;
	menu_return();
}

char* menu_beep_keys_get_val(uint8_t unused)
{
	return pgmtoa(config.beep_keys ? messages[ON_STR]:messages[OFF_STR]);
}

void menu_ppm_toggle(void)
{
	ppm_status.enabled ^= 1;
	menu_return();
}

char* menu_ppm_get_val(uint8_t unused)
{
	return pgmtoa(ppm_status.enabled? messages[ON_STR]:messages[OFF_STR]);
}



void menu_lowbatt_start(void)
{
	menu_simple_lister_start(&config.low_batt,0, VCC_MAX_VOLTAGE_mV/100, menu_lowbatt_draw);
}
void menu_lowbatt_draw(uint8_t val)
{
	menu_items_draw_value(MENU_ACTIVE_LINE, itoa_dec(val,MENU_NUMBER_LENGTH,1),OVERWRITE|INVERT);
}
char* menu_lowbatt_getval(uint8_t unused)
{
	return itoa_dec(config.low_batt,MENU_NUMBER_LENGTH,1);
}
