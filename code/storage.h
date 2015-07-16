#ifndef _STORAGE_LIB
	#define _STORAGE_LIB

	#include <inttypes.h>

	#include "config.h"
	#include "input.h"
	#include "ppm.h"
	#include "menu_model.h"

	//if check flags
	#define EEID_OK					0
	#define EEID_SUBVER_MISMATCH	(1<<0)
	#define EEID_VER_MISMATCH		(1<<1)
	#define EEID_FW_MISMATCH		(1<<2)
	#define EEID_VENDOR_MISMATCH	(1<<3)

	//address defines
	//identification
	#define EAD_FW_ID			0
	#define EAD_VENDOR_ID		1
	#define EAD_VER				2
	#define EAD_SUBVER			3
	#define EAD_CONFIG     		4 //2 bytes
	#define EAD_MODEL_SELECTED	6

	#define EAD_INPUT_BASE	7
	#define EAD_INPUT_SIZE	8 //sizeof(input) was 8
	#define EAD_INPUT_GET_ADDRESS(A) ((uint16_t)A*EAD_INPUT_SIZE + EAD_INPUT_BASE)
	#define EAD_INPUT_END	(EAD_INPUT_BASE+(EAD_INPUT_SIZE*IN_COUNT))

	//<FLAG> <NAME> <OUTPUTS> <IN_TYPE_LEVELING LEVELS> <IN_TYPE_TOGGLING BOUNDS>
	#define EAD_MODEL_BASE			48 //INPUTS END is 7+5*17, round it up... 100
	#define EAD_MODEL_FLAG_SIZE		1
	#define EAD_MODEL_NAME_SIZE		MODEL_MAX_NAME_LENGTH
	#define EAD_MODEL_INPUT_SIZE	3
	#define EAD_MODEL_OUTPUT_SIZE	10
	#define EAD_MODEL_INPUT_LEVELING_TABLE_SIZE	IN_LEVELING_MAX_LEVELS
	#define EAD_MODEL_INPUT_TOGGLING_BOUNDS_SIZE	2 //(min+ max)
	#define EAD_MODEL_OUTPUT_COUNT	PPM_OUTPUT_COUNT
	#define EAD_MODEL_ALIGNMENT		1 //could be 0, just unused space, useful for debugging, FF is easy to spot in hexdump...
	#define EAD_MODEL_SLOT_SIZE		((EAD_MODEL_OUTPUT_SIZE*PPM_OUTPUT_COUNT) + EAD_MODEL_FLAG_SIZE + EAD_MODEL_NAME_SIZE + (EAD_MODEL_INPUT_SIZE*IN_COUNT) + EAD_MODEL_INPUT_TOGGLING_BOUNDS_SIZE + EAD_MODEL_INPUT_LEVELING_TABLE_SIZE + EAD_MODEL_ALIGNMENT)
	#define EAD_MODEL_GET_SLOT_ADDRESS(S)	(EAD_MODEL_BASE+ (uint16_t)S*EAD_MODEL_SLOT_SIZE)

	#if EAD_MODEL_BASE + (MODEL_MAX_COUNT)* EAD_MODEL_SLOT_SIZE >= 1024
		#error "Can't fit so many model slots in EEPROM!"
	#endif

	#define E8(A)	((uint8_t*)A)
	#define E16(A)	((uint16_t*)A)

	void storage_wipe_all(void);
	void storage_dump_eeprom(void);

	void storage_all_store(void);
	void storage_all_load(void);
	void storage_global_store(void);
	void storage_global_load(void);
	void storage_id_store(void);
	uint8_t storage_id_check(void);
	void storage_config_store(void);
	void storage_config_load(void);
	void storage_input_config_store(void);
	void storage_input_config_load(void);

	uint8_t storage_model_load(uint8_t slot_num);
	void storage_model_store(uint8_t slot_num);
	void storage_model_initial_load(void);
	void storage_model_actual_store(void);
	char *storage_model_get_name(uint8_t slot_num);


#endif
