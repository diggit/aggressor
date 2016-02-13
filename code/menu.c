#include "menu.h"
#include "control.h"
#include "3310_routines.h"
#include "menu_structure.h"
#include "systick.h"
#include "homescreen.h"
#include "misc.h"



//some useful, multiple times used messages
const char on_str[] PROGMEM={"ON"};
const char off_str[] PROGMEM={"OFF"};
const char *messages[]=
{
	[ON_STR]=on_str,
	[OFF_STR]=off_str
};

menu_p menu;

menu_tmp_t menu_tmp;

void menu_clear_tmp()
{
	menu_tmp.u16p=0;//clearing pointer should be enough
}

//called to initialize main menu
void menu_start()
{
	menu=&main_menu;
	menu_return();
}

void menu_return()
{
	menu_clear_tmp();
	fiftyps_periodical_call=NULL;
	menu_state=MENU_STATE_MENU;
	set_event_listener(menu_button_listener);
	set_new_drawer(menu_draw);
}

void menu_button_listener(uint8_t buttons)
{
	menu_items_p item;

	switch (buttons) {
		case BTN_INC:
			if(menu->hilighted)
			{
				menu->hilighted--;
				menu_items_draw();
			}
			break;

		case BTN_DEC:
			if(menu->hilighted < menu->items_total-1)
			{
				menu->hilighted++;
				menu_items_draw();
			}
			break;

		case BTN_FUNC:
			if(menu->parent)//not NULL
				menu_enter_parent_menu();
			else
				homescreen_return();
			break;

		case BTN_SEL:
			item=&menu->items[menu->hilighted];

			if(item->handler_func)//not NULL
			{
				menu_state=MENU_STATE_HANDLER;
				item->handler_func();
			}
			if(item->submenu)//not NULL
			{
				menu_enter_submenu(menu->items[menu->hilighted].submenu);
			}
			break;

		case BTN_FUNC_LONG: //from any deep menu, there is a quick way to idle screen
			homescreen_return();
			break;
	}
}

void menu_enter_parent_menu()
{
	menu=menu->parent;
	menu_draw();
}

void menu_enter_submenu(struct menu_s *submenu)
{
	menu_state=MENU_STATE_MENU;
	submenu->parent=menu;//set reference that this is submenus parent
	menu=submenu;//switch reference to submenu
	menu_draw();
}

//draws visible items in menu
void menu_items_draw()
{
	LCD_clear_partial();
	menu_items_p item;
	for (uint8_t line = 2; line < 6; line++)
	{
		if(line==3)
		{
			LCD_gotoXY(0,3);
			LCD_writeChar('>',1);
		}
		if((menu->hilighted + line>2) && (line+menu->hilighted < menu->items_total+3)) //visible items
		{
			item=&menu->items[line+menu->hilighted - 3];
			if(item->name) //not NULL
			{
				LCD_writeString_XY(MENU_NAME_COLUMN,line,pgmtoa(item->name),OVERWRITE);
				if (menu_state==MENU_STATE_VALMOD && line==MENU_ACTIVE_LINE)
				{
					button_event_listener(BTN_NONE);//force redraw, when value beeing changed
				}
				else
				{
					if(item->get_value)
						menu_items_draw_value(line,item->get_value(line+menu->hilighted - 3),OVERWRITE); //tell, which element in menu is it...
				}
			}
			else if(item->get_value)//use as dynamic name if not static given, as parameter give position in menu
				LCD_writeString_XY(MENU_NAME_COLUMN,line,item->get_value(line+menu->hilighted - 3),OVERWRITE);
		}
	}
}

void menu_items_draw_value(uint8_t line,char *value,uint8_t flags)
{
	LCD_gotoXY(MENU_VALUE_COLUMN,line);
	for (uint8_t ch = 0; ch < 4; ch++) {
		LCD_writeChar(' ',OVERWRITE);
	}
	LCD_writeString_XY(MENU_VALUE_COLUMN,line,value,flags);
}

void menu_draw()
{
	if(menu_state==MENU_STATE_MENU || menu_state==MENU_STATE_VALMOD)
	{
		menu_template_draw();
		menu_items_draw();
	}
}

void menu_template_draw()
{
	LCD_clear_partial();
	if(menu->name) //not NULL
	{
		LCD_write_header_text(pgmtoa(menu->name));
	}
	// if(menu->parent) //not NULL
	// {
	// 	//fallback
	// 	LCD_write_header_text(menu->parent->items[menu->parent->hilighted].name);
	// }
	else if( menu->parent->items[menu->parent->hilighted].name )//alternatively use name as it was listed in parent menu
		LCD_write_header_text(pgmtoa(menu->parent->items[menu->parent->hilighted].name));
	else if( menu->parent->items[menu->parent->hilighted].get_value )//last chance is to use dynamic name, it it had no static name in parent menu
		LCD_write_header_text(menu->parent->items[menu->parent->hilighted].get_value(menu->parent->hilighted));
}



mixed8_t lister_max, lister_min, lister_value_copy;

mixed8_p lister_original_value_p;

union{
	void (*s)(int8_t val);//callback func, that receives value
	void (*u)(uint8_t val);//callback func, that receives value
}texter_lister_callback;


void (*lister_finish_callback)(void); //final callback, when leaving lister
//UNSIGNED VERSION

void menu_simple_lister_rapid_inc(void)
{
	menu_simple_lister_listener(BTN_INC);//simulate BTN_INC pressing
}

void menu_simple_lister_rapid_dec(void)
{
	menu_simple_lister_listener(BTN_DEC);//simulate BTN_INC pressing
}

void menu_simple_lister_start_ex(uint8_t *value_p,uint8_t min, uint8_t max, void (*callback)(uint8_t val), void (*finish_callback)(void))
{
	menu_state=MENU_STATE_VALMOD;
	set_event_listener(menu_simple_lister_listener);
	if(*value_p>max)
		lister_value_copy.u=max;
	else if(*value_p<min)
		lister_value_copy.u=min;
	else
		lister_value_copy.u=*value_p; //save original value

	lister_original_value_p.u=value_p;//save pointer to original data, to commit changes on confirmation
	lister_max.u=max;
	lister_min.u=min;
	texter_lister_callback.u=callback;
	lister_finish_callback=finish_callback;
	if(texter_lister_callback.u)
		texter_lister_callback.u(lister_value_copy.u);//first draw
	else //fallback, just print value
		menu_items_draw_value(MENU_ACTIVE_LINE,itoa(lister_value_copy.u,MENU_NUMBER_LENGTH),OVERWRITE|INVERT);
}

void menu_simple_lister_listener(uint8_t buttons)
{
	if(buttons == BTN_INC_LONG)
	{
		fiftyps_periodical_call=menu_simple_lister_rapid_inc;
	}
	else if(buttons == BTN_DEC_LONG)
	{
		fiftyps_periodical_call=menu_simple_lister_rapid_dec;
	}
	else if(buttons == BTN_INC)
	{
		if(lister_value_copy.u<lister_max.u)
			lister_value_copy.u++;

	}
	else if(buttons == BTN_DEC)
	{
		if(lister_value_copy.u>lister_min.u)
			lister_value_copy.u--;
	}
	else
	{
		fiftyps_periodical_call=NULL;
	}



	if(buttons & (BTN_INC|BTN_DEC) || buttons == BTN_NONE) //if value changed or BTNS zero indicating forced update
	{
		if(texter_lister_callback.u)
			texter_lister_callback.u(lister_value_copy.u);//first draw
		else //fallback, just print value
			menu_items_draw_value(MENU_ACTIVE_LINE,itoa(lister_value_copy.u,MENU_NUMBER_LENGTH),OVERWRITE|INVERT);

	}

	//finish value changing
	if(buttons == BTN_SEL)
	{
		*lister_original_value_p.u=lister_value_copy.u;
		if(lister_finish_callback)
			lister_finish_callback();
		menu_return();
	}
	else if(buttons == BTN_FUNC)
	{
		if(lister_finish_callback)
			lister_finish_callback();
		menu_return();
	}
}

uint8_t menu_simple_lister_get_copy()
{
	return lister_value_copy.u;
}


//SIGNED VERSION
void menu_simple_lister_signed_rapid_inc(void)
{
	menu_simple_lister_signed_listener(BTN_INC);//simulate BTN_INC pressing
}

void menu_simple_lister_signed_rapid_dec(void)
{
	menu_simple_lister_signed_listener(BTN_DEC);//simulate BTN_INC pressing
}

void menu_simple_lister_signed_start_ex(int8_t *value_p,int8_t min, int8_t max, void (*callback)(int8_t val), void (*finish_callback)(void))
{
	menu_state=MENU_STATE_VALMOD;
	set_event_listener(menu_simple_lister_signed_listener);
	if(*value_p>max)
		lister_value_copy.s=max;
	else if(*value_p<min)
		lister_value_copy.s=min;
	else
		lister_value_copy.s=*value_p; //save original value

	lister_original_value_p.s=value_p;//save pointer to original data, to commit changes on confirmation
	lister_max.s=max;
	lister_min.s=min;
	texter_lister_callback.s=callback;
	lister_finish_callback=finish_callback;
	if(texter_lister_callback.s)
		texter_lister_callback.s(lister_value_copy.s);//first draw
	else //fallback, just print value
		menu_items_draw_value(MENU_ACTIVE_LINE,itoa(lister_value_copy.s,MENU_NUMBER_LENGTH),OVERWRITE|INVERT);
}

void menu_simple_lister_signed_listener(uint8_t buttons)
{
	if(buttons == BTN_INC_LONG)
	{
		fiftyps_periodical_call=menu_simple_lister_signed_rapid_inc;
	}
	else if(buttons == BTN_DEC_LONG)
	{
		fiftyps_periodical_call=menu_simple_lister_signed_rapid_dec;
	}
	else if(buttons == BTN_INC)
	{
		if(lister_value_copy.s<lister_max.s)
			lister_value_copy.s++;

	}
	else if(buttons == BTN_DEC)
	{
		if(lister_value_copy.s>lister_min.s)
			lister_value_copy.s--;
	}
	else
	{
		fiftyps_periodical_call=NULL;
	}

	if(buttons & (BTN_INC|BTN_DEC) || buttons == BTN_NONE) //if value changed or BTNS zero indicating forced update
	{
		if(texter_lister_callback.s)
			texter_lister_callback.s(lister_value_copy.s);//first draw
		else //fallback, just print value
			menu_items_draw_value(MENU_ACTIVE_LINE,itoa(lister_value_copy.s,MENU_NUMBER_LENGTH),OVERWRITE|INVERT);
	}

	//finish value changing
	if(buttons == BTN_SEL)
	{
		*lister_original_value_p.s=lister_value_copy.s;
		if(lister_finish_callback)
			lister_finish_callback();
		menu_return();
	}
	else if(buttons == BTN_FUNC)
	{
		if(lister_finish_callback)
			lister_finish_callback();
		menu_return();
	}
}

int8_t menu_simple_lister_signed_get_copy()
{
	return lister_value_copy.s;
}


// #define TEXT_ABS_start			' '
// #define TEXT_ABS_end			'~'
// #define TEXT_LETTERS_start		'a'
// #define TEXT_LETTERS_end		'z'
#define TEXT_LETTERS_CAP_start	'A'
#define TEXT_LETTERS_CAP_end	'Z'
#define TEXT_NUMBERS_start		'0'
#define TEXT_NUMBERS_end		'9'

uint8_t texter_length,texter_selected;
char* texter_text_p;
//order  [A-Z] [0-9] [ ]


char menu_simple_texter_next_char_grp(char c)
{


	if (c==TEXT_LETTERS_CAP_end)
	{
		return TEXT_NUMBERS_start;
	}
	else if (c==TEXT_NUMBERS_end)
	{
		return ' ';
	}
	else if(c==' ')
		return TEXT_LETTERS_CAP_start;
	else
		return c+1;
}

char menu_simple_texter_prev_char_grp(char c)
{
	if (c==TEXT_LETTERS_CAP_start)
	{
		return ' ';
	}
	else if (c==TEXT_NUMBERS_start)
	{
		return TEXT_LETTERS_CAP_end;
	}
	else if(c==' ')
		return TEXT_NUMBERS_end;
	else
		return c-1;
}

uint8_t menu_simple_texter_is_char_valid(char c)
{
	return (TEXT_NUMBERS_start <= c && c <= TEXT_NUMBERS_end) || (TEXT_LETTERS_CAP_start <=c && c <= TEXT_LETTERS_CAP_end);
}

void menu_simple_texter_start(char *text_p, uint8_t length, void (*callback)(uint8_t val))
//length is number of chars to be modified, it si extected, that refferenced buffer is ath lest one byte longer for closing \0
{
	menu_state=MENU_STATE_VALMOD;
	texter_length=length;
	texter_text_p=text_p;
	texter_selected=0;
	texter_lister_callback.u=callback;
	for (uint8_t i = 0; i < texter_length; i ++) //check for non valid characters and replace them with space
	{
		if( !(menu_simple_texter_is_char_valid(texter_text_p[i])) )
		{
			if(i==0)
				texter_text_p[i]='A';
			else
				texter_text_p[i]=' ';
		}
	}
	texter_text_p[length]='\0';//set last character to terminal
	set_event_listener(menu_simple_texter_listener);
	if(texter_lister_callback.u)
		menu_simple_texter_listener(BTN_NONE);//just for first draw
}

void menu_simple_texter_listener(uint8_t buttons)
{
	uint8_t ch;
	switch(buttons)
	{
		case BTN_INC:
			texter_text_p[texter_selected]=menu_simple_texter_next_char_grp(texter_text_p[texter_selected]);
			break;
		case BTN_DEC:
			texter_text_p[texter_selected]=menu_simple_texter_prev_char_grp(texter_text_p[texter_selected]);
			break;
		case BTN_SEL://select between characters
			if(texter_selected<MODEL_MAX_NAME_LENGTH-1)
				texter_selected++;
			else
				texter_selected=0;
			break;
		case BTN_FUNC:
			for (ch = texter_length; ch > 0 && (texter_text_p[ch]==' ' || texter_text_p[ch]=='\0') ; ch--)//translate all trailing spaces to \0 terminating chars
			{
				texter_text_p[ch]='\0';
			}
			menu_return();
			return;
			break;
	}
	if(texter_lister_callback.u)
		texter_lister_callback.u(texter_selected);
}
