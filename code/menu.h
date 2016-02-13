#ifndef __MENU_LIB
#define __MENU_LIB

	#include <inttypes.h>

	#include "config.h"
	#include "menu_structure.h"//not needed in header, but mostly useful with menu itself
	#include "misc.h"

	#define MENU_STATE_IDLE		0
	#define MENU_STATE_MENU		1
	#define MENU_STATE_HANDLER	2
	#define MENU_STATE_VALMOD	3

	#define MENU_ACTIVE_LINE	3
	#define MENU_VALUE_COLUMN	60
	#define MENU_NAME_COLUMN	6

	#define MENU_NUMBER_LENGTH	4

	//ho much pace header text have from 0,0? (in characters)
	#if !defined MENU_HEADER_LENGTH
		#error "MENU_HEADER_LENGTH not defined"
	#endif

	extern uint8_t menu_state;

	extern struct menu_s *menu;

	extern const char *messages[];
	#define ON_STR	0
	#define OFF_STR 1

	void menu_clear_tmp(void);
	void menu_start(void);
	void menu_return(void);
	void menu_button_listener(uint8_t buttons);
	void menu_enter_parent_menu(void);
	void menu_enter_submenu(struct menu_s *submenu);
	void menu_items_draw(void);
	void menu_items_draw_value(uint8_t line,char *value,uint8_t flags);
	void menu_draw(void);
	void menu_template_draw(void);

	typedef union {
		uint8_t u;
		int8_t s;
	} mixed8_t;
	extern mixed8_t lister_value_copy;

	typedef union {
		uint8_t *u;
		int8_t *s;
	} mixed8_p;
	extern mixed8_p lister_original_value_p;

	typedef union{
		uint8_t u8;
		int8_t s8;
		uint8_t volatile *u8p;
		int8_t volatile *s8p;
		uint16_t u16;
		int16_t s16;
		uint16_t volatile *u16p;
		int16_t volatile *s16p;
	} menu_tmp_t;
	extern menu_tmp_t menu_tmp;

	void menu_simple_lister_rapid_inc(void);
	void menu_simple_lister_rapid_dec(void);
	void menu_simple_lister_start_ex(uint8_t *value_p,uint8_t min, uint8_t max, void (*callback)(uint8_t val), void (*finish_callback)(void));
	#define menu_simple_lister_start(A,B,C,D)	menu_simple_lister_start_ex(A,B,C,D,NULL)
	void menu_simple_lister_listener(uint8_t buttons);
	uint8_t menu_simple_lister_get_copy(void);

	void menu_simple_lister_signed_rapid_inc(void);
	void menu_simple_lister_signed_rapid_dec(void);
	void menu_simple_lister_signed_start_ex(int8_t *value_p,int8_t min, int8_t max, void (*callback)(int8_t val), void (*finish_callback)(void));
	#define menu_simple_lister_signed_start(A,B,C,D)	menu_simple_lister_signed_start_ex(A,B,C,D,NULL)
	void menu_simple_lister_signed_listener(uint8_t buttons);
	int8_t menu_simple_lister_signed_get_copy(void);

	char menu_simple_texter_next_char_grp(char c);
	char menu_simple_texter_prev_char_grp(char c);
	uint8_t menu_simple_texter_is_char_valid(char c);
	void menu_simple_texter_start(char *text_p, uint8_t length, void (*callback)(uint8_t val));
	void menu_simple_texter_listener(uint8_t buttons);



#endif
