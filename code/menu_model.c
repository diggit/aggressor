#include "menu_model.h"
#include <avr/pgmspace.h>
#include "3310_routines.h"
#include "menu.h"
#include "output.h"
#include "storage.h"

char model_name[MODEL_MAX_NAME_LENGTH+1];
uint8_t model_selected;

const char noname_str[] PROGMEM={"NONAME"};

void model_load_defaults(void)
{
	output_load_defaults();
	strcpy(model_name,pgmtoa(noname_str));
}

// char *menu_model_name_get()
// {
// 	return model_name;
// }

void menu_model_name_draw(uint8_t hilighted)
{
	uint8_t i=0;
	LCD_gotoXY(MENU_NAME_COLUMN, MENU_ACTIVE_LINE);
	while(i<=MODEL_MAX_NAME_LENGTH && model_name[i]!='\0')
	{
		if(hilighted==i)
			LCD_writeChar(model_name[i],OVERWRITE);
		else
			LCD_writeChar(model_name[i],INVERT|OVERWRITE);
		i++;
	}
}

void menu_model_name_change(void)
{
	menu_simple_texter_start(model_name,MODEL_MAX_NAME_LENGTH,menu_model_name_draw);
}

char *menu_model_name_getval(uint8_t unused)
{
	return model_name;
}



//COPY model to slot...
void menu_model_copy_to_start(void)
{
	menu_tmp.u8=model_selected;
	menu_simple_lister_start_ex(&menu_tmp.u8,0 , MODEL_MAX_COUNT-1 , menu_model_draw, menu_model_copy_to_finish);
}

const char empty_str[] PROGMEM={"empty"};

void menu_model_draw(uint8_t val)
{
	LCD_clear_row(MENU_ACTIVE_LINE);
	LCD_writeString_XY(MENU_NAME_COLUMN,MENU_ACTIVE_LINE,itoa(val,2),0); //number of slot

	uint8_t flags=0;
	char *name;

	if(val==model_selected)
	{
		flags|=INVERT;
		name=model_name;
	}
	else
	{
		name=storage_model_get_name(val);
		if(name==NULL)
		{
			name=pgmtoa(empty_str);
		}
	}
	LCD_writeString_XY(20,MENU_ACTIVE_LINE,name,flags);
}

void menu_model_copy_to_finish(void)
{
	if(model_selected != menu_tmp.u8)//selected different slot than active one
	{
		storage_model_store(menu_tmp.u8);
	}
}

//LOAD model from slot
void menu_model_load_start(void)
{
	menu_tmp.u8=model_selected;
	menu_simple_lister_start_ex(&menu_tmp.u8,0 , MODEL_MAX_COUNT-1 , menu_model_draw, menu_model_load_finish);
}

void menu_model_load_finish(void)
{
	if(model_selected != menu_tmp.u8)//selected different slot than active one
	{
		storage_model_store(model_selected);//save actual model
		model_selected=menu_tmp.u8;//set new slot
		if(storage_model_load(model_selected)==0)//iv non valid data in slot...
		{
			model_load_defaults();
			storage_model_store(model_selected);//and store them
		}
		storage_model_actual_store();//and save which model is selected now
	}
}

void menu_model_load_defaults(void)
{
	model_load_defaults();
	menu_return();
}

void menu_model_delete_start(void)
{
	menu_tmp.u8=model_selected;
	menu_simple_lister_start_ex(&menu_tmp.u8,0 , MODEL_MAX_COUNT-1 , menu_model_draw, menu_model_delete_finish);
}

void menu_model_delete_finish(void)
{
	if(model_selected != menu_tmp.u8)//selected different slot than active one
	{
		eeprom_update_byte(E8(EAD_MODEL_GET_SLOT_ADDRESS(menu_tmp.u8)), MODEL_FLAG_VALID^0xff );//just overwrite validity flag (its inversion is most secure)
	}
}
