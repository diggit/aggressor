#include "storage.h"
#include "input.h"
#include "output.h"
#include "hardware.h"
#include "misc.h"
#include "uart.h"
#include <avr/eeprom.h>
#include <avr/io.h>
#include "trim.h"

void storage_wipe_all(void)
{
	for (uint16_t b = 0; b < 1024; b++) {
		eeprom_update_byte(E8(b),0xff);
	}
	beep(BEEP_NOTIF);
}

void storage_dump_eeprom(void)
{
	for (uint16_t b = 0; b < 1024; b++) {
		uart_putc(eeprom_read_byte(E8(b)));
	}
	beep(BEEP_NOTIF);
}

void storage_all_store()
{
	storage_id_store();
	storage_config_store();
	storage_input_config_store();
	storage_model_actual_store();
}

void storage_all_load()
{
	uint8_t storage_status=storage_id_check();
	if(storage_status==EEID_OK || storage_status==EEID_SUBVER_MISMATCH)
	{
		storage_config_load();
		storage_input_config_load();
		storage_model_initial_load();
	}
	else
	{
		model_load_defaults();
		return;//do not read anything else, eeprom not written yet
	}

}




void storage_id_store()
{
	eeprom_update_byte(E8(EAD_FW_ID),FW_ID);
	eeprom_update_byte(E8(EAD_VENDOR_ID),VENDOR_ID);
	eeprom_update_byte(E8(EAD_VER),_ver);
	eeprom_update_byte(E8(EAD_SUBVER),_subver);
}

uint8_t storage_id_check()
{
	uint8_t out=EEID_OK;
	if(eeprom_read_byte(E8(EAD_FW_ID)) != FW_ID)
		out|=EEID_FW_MISMATCH;
	if(eeprom_read_byte(E8(EAD_VENDOR_ID)) != VENDOR_ID)
		out|=EEID_VENDOR_MISMATCH;
	if(eeprom_read_byte(E8(EAD_VER)) != _ver)
		out|=EEID_VER_MISMATCH;
	if(eeprom_read_byte(E8(EAD_SUBVER)) != _subver)
		out|=EEID_SUBVER_MISMATCH;

	return out;
}

void storage_config_store()
{
	eeprom_update_block(&config,E8(EAD_CONFIG),sizeof(config));
}

void storage_config_load()
{
	eeprom_read_block(&config,E8(EAD_CONFIG),sizeof(config));
}

void storage_input_config_store()
{
	for(uint16_t in_num=0;in_num<array_length(inputs);in_num++)
	{
		eeprom_update_block(&inputs[in_num].global,E8(EAD_INPUT_GET_ADDRESS(in_num)),EAD_INPUT_SIZE);
	}
}

void storage_input_config_load()
{
	input in_tmp;
	for(uint8_t in_num=0;in_num<array_length(inputs);in_num++)
	{
		eeprom_read_block(&in_tmp.global,E8(EAD_INPUT_GET_ADDRESS(in_num)),EAD_INPUT_SIZE);
		if(in_tmp.global.common.type==inputs[in_num].global.common.type )
		{
			inputs[in_num].global=in_tmp.global;//copy verified struct from eeprom
		}
		else
		{
			beep(BEEP_ERROR);
		}
	}
}

void storage_model_actual_store(void)
{
	eeprom_update_byte(E8(EAD_MODEL_SELECTED),model_selected);
	storage_model_store(model_selected);
}

char *storage_model_get_name(uint8_t slot_num)
{
	uint16_t addr=(EAD_MODEL_GET_SLOT_ADDRESS(slot_num));

	if(eeprom_read_byte(E8(addr)) != MODEL_FLAG_VALID) //not valid flag, empty slot of invalid version
		return NULL;

	addr+=EAD_MODEL_FLAG_SIZE;
	return etoa(addr,MODEL_MAX_NAME_LENGTH);
}

void storage_model_store(uint8_t slot_num)
{
	uint16_t addr=(EAD_MODEL_GET_SLOT_ADDRESS(slot_num));

	eeprom_update_byte(E8(addr),MODEL_FLAG_VALID);
	addr+=EAD_MODEL_FLAG_SIZE;

	eeprom_update_block(model_name,E8(addr),EAD_MODEL_NAME_SIZE);
	addr+=EAD_MODEL_NAME_SIZE;

	for (uint16_t out_num=0;out_num<EAD_MODEL_OUTPUT_COUNT;out_num++)
	{
		eeprom_update_block(&outputs[out_num],E8(addr),EAD_MODEL_OUTPUT_SIZE);
		addr+=EAD_MODEL_OUTPUT_SIZE;
	}
	for (uint8_t in_num = 0; in_num < IN_COUNT; in_num++)
	{
		eeprom_update_block(&inputs[in_num].model,E8(addr),EAD_MODEL_INPUT_SIZE);
		addr+=EAD_MODEL_INPUT_SIZE;
	}

	//expected exactly 1 leveling input and exactly 1 !
	eeprom_update_block(in_alt_levels,E8(addr),EAD_MODEL_INPUT_LEVELING_TABLE_SIZE);
	addr+=EAD_MODEL_INPUT_LEVELING_TABLE_SIZE;
	eeprom_update_byte(E8(addr),inputs[IN_THUMB].global.digital.min);
	addr++;
	eeprom_update_byte(E8(addr),inputs[IN_THUMB].global.digital.max);
}

uint8_t storage_model_load(uint8_t slot_num)
{
	uint16_t addr=(EAD_MODEL_GET_SLOT_ADDRESS(slot_num));

	if(eeprom_read_byte(E8(addr)) != MODEL_FLAG_VALID) //not valid flag, empty slot of invalid version
		return 0;
	addr+=EAD_MODEL_FLAG_SIZE;

	eeprom_read_block(model_name,E8(addr),EAD_MODEL_NAME_SIZE);
	model_name[EAD_MODEL_NAME_SIZE]='\0';//force last char to be terminating \0
	addr+=EAD_MODEL_NAME_SIZE;

	for (uint16_t out_num=0;out_num<EAD_MODEL_OUTPUT_COUNT;out_num++)
	{
		eeprom_read_block(&outputs[out_num],E8(addr),EAD_MODEL_OUTPUT_SIZE);
		addr+=EAD_MODEL_OUTPUT_SIZE;
	}
	for (uint8_t in_num = 0; in_num < IN_COUNT; in_num++)
	{
		eeprom_read_block(&inputs[in_num].model,E8(addr),EAD_MODEL_INPUT_SIZE);
		addr+=EAD_MODEL_INPUT_SIZE;
	}
	//expected exactly 1 leveling input and exactly 1 toggling input!
	eeprom_read_block(in_alt_levels,E8(addr),EAD_MODEL_INPUT_LEVELING_TABLE_SIZE);
	addr+=EAD_MODEL_INPUT_LEVELING_TABLE_SIZE;
	//override raw min/max for toggling input, zhis is model specific
	inputs[IN_THUMB].global.digital.min=(int8_t)eeprom_read_byte(E8(addr));
	addr++;
	inputs[IN_THUMB].global.digital.max=(int8_t)eeprom_read_byte(E8(addr));

	return 1;
}

void storage_model_initial_load(void)
{
	model_selected=eeprom_read_byte(E8(EAD_MODEL_SELECTED));
	if(model_selected>MODEL_MAX_COUNT)//value out of range, do not load any data
	{
		model_selected=0;
		return;
	}

	if(storage_model_load(model_selected)==0)
	{
		model_load_defaults();
	}


}
