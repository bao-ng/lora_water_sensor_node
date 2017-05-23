#include <stdlib.h>
#include <string.h>
#include "app.h"

#include "console.h"
#include "cmd_line.h"
#include "lora_sx1276.h"
#include "capacitive_measure.h"

#include "../platform/spi/spi.h"
#include "../platform/flash/flash.h"

static int32_t i_lgn_help(uint8_t* argv);
static int32_t i_lgn_lora(uint8_t* argv);
static int32_t i_lgn_fi(uint8_t* argv);
static int32_t i_lgn_sensor(uint8_t* argv);

cmd_line_t lgn_cmd_table[] = {
	{(const int8_t*)"help",		i_lgn_help,			(const int8_t*)"cmd info"},
	{(const int8_t*)"lora",		i_lgn_lora,			(const int8_t*)"lora cmd"},
	{(const int8_t*)"fi",		i_lgn_fi,			(const int8_t*)"set default"},
	{(const int8_t*)"calib",	i_lgn_sensor,		(const int8_t*)"calibrate sensor"},


	/* End Of Table */
	{(const int8_t*)0,(pf_cmd_func)0,(const int8_t*)0}
};

int32_t i_lgn_help(uint8_t* argv) {
	uint32_t idx = 0;
	switch (*(argv + 4)) {
	default:
		APP_DBG("\nCOMMANDS INFORMATION:\n\n");
		while(lgn_cmd_table[idx].cmd != (const int8_t*)0) {
			APP_DBG("%s\n\t%s\n\n", lgn_cmd_table[idx].cmd, lgn_cmd_table[idx].info);
			idx++;
		}
		break;
	}
	return 0;
}

static uint8_t set_flag;
static uint16_t set_value;
int32_t i_lgn_lora(uint8_t *argv){
	/* lora h 1234*/ //host
	/* lora s 1234*/ //slave
	switch (*(argv + 5)) {
	case 'h': {
		if (strlen((const char*)argv) >= 11) {
			*(argv + 11) = 0;
			set_value = atoi((const char*)(argv + 7));
			set_flag = 0;
			APP_DBG("\nDo you want to write Address = %d to flash [Y]/[N]\n",(uint32_t)set_value);
		}
		else {
			APP_DBG("invalib cmd\n");
		}
	}
		break;

	case 's': {
		if (strlen((const char*)argv) >= 11) {
			*(argv + 11) = 0;
			set_value = atoi((const char*)(argv + 7));
			set_flag = 1;
			APP_DBG("\nDo you want to write Address = %d to flash [Y]/[N]\n",(uint32_t)set_value);
		}
		else {
			APP_DBG("invalib cmd\n");
		}
	}
		break;

	case 'y': {
		switch (set_flag) {
		case 0: {
			flash_package_rw_t flash_package;
			get_lora_cfg(&flash_package);
			flash_package.my_node_addr = set_value;
			set_lora_cfg(&flash_package);
		}
			break;

		case 1: {
			flash_package_rw_t flash_package;
			get_lora_cfg(&flash_package);
			flash_package.my_server_addr = set_value;
			set_lora_cfg(&flash_package);
		}
			break;

		default:
			break;
		}

		APP_DBG("OK\n");
	}
		break;

	case 'n':
		break;

	default:
		APP_DBG("unknown option\n");
		break;
	}
	return 0;
}

static int32_t i_lgn_fi(uint8_t* argv) {
	(void)argv;
	flash_erase_sector(FLASH_CFG_ADDR);
}

static int32_t i_lgn_sensor(uint8_t* argv) {
	(void)argv;
	uint32_t calib_val = 0;
	calib_val = measure_water_level(WATER_LEVEL_1);
	for(int i =0; i<10; i++) {
		calib_val =(calib_val+measure_water_level(WATER_LEVEL_1))/2;
	}

	flash_package_rw_t flash_package;
	get_lora_cfg(&flash_package);
	flash_package.sensor_calib_value = calib_val;
	set_lora_cfg(&flash_package);

	APP_DBG("calib value : %d\n",calib_val);

}
