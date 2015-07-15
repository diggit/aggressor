#ifndef _MODEL_MENU_LIB
	#define _MODEL_MENU_LIB

	#include <inttypes.h>
	#include <string.h>

	#include "config.h"
	#include "menu_model.h"

	#define MODEL_MAX_COUNT	10
	#define MODEL_FLAG_VALID 0xaa //some value, change when output structure changes to discard saved incompatible data

	extern char model_name[MODEL_MAX_NAME_LENGTH+1];
	extern uint8_t model_selected;

	void model_load_defaults(void);

	void menu_model_name_draw(uint8_t hilighted);
	void menu_model_name_change(void);
	char *menu_model_name_getval(uint8_t unused);

	void menu_model_copy_to_start(void);
	void menu_model_draw(uint8_t val);
	void menu_model_copy_to_finish(void);

	void menu_model_load_start(void);
	void menu_model_load_finish(void);

	void menu_model_load_defaults(void);

	void menu_model_delete_start(void);
	void menu_model_delete_finish(void);

#endif
