#ifndef _MENU_STRUCTUR_LIB
	#define _MENU_STRUCTUR_LIB

	#include <inttypes.h>

	struct menu_s;

	typedef  struct{
		const char *name;
		void (*handler_func)(void);
		char* (*get_value)(uint8_t val);
		struct menu_s *submenu;
	}menu_items;

	typedef menu_items *menu_items_p;

	struct menu_s
	{
		const char *name;
		menu_items *items;
		uint8_t items_total;
		uint8_t hilighted;
		struct  menu_s *parent;
	};

	typedef struct menu_s menu_t;
	typedef menu_t *menu_p;

	extern menu_t main_menu;

	menu_t main_menu, output_menu, input_menu, output_properties_menu, input_properties_menu, output_inmix_menu, model_menu,leveling_input_menu,_input_menu;

	void menu_structure_init(void);

#endif
