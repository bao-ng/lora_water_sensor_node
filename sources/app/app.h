#ifndef __APP_H__
#define __APP_H__

#include <stdint.h>
#include "cmd_line.h"

#define APP_DBG_EN

#define LORA_MESSAGE_DATA_SIZE		(4)
#define LORA_ADDRESS_SIZE			(4)

#define WATER_LEVEL_HIGH			(2500)

#define FLASH_CFG_ADDR				(0x1000)

#define TYPE_CFG					(0)
#define TYPE_REPORT					(1)
#define TYPE_KEEP_ALIVE				(2)
#define SERVER_CONFIRM				(1)

#define ONE_DAY						(60)//(2880)
#define THIRTY_SECONDS				(1)

#define LORA_PAIRED_STATE			(1)
#define LORA_CONFIG_STATE			(0)

#define WATER_LEVEL_1				(1)
#define WATER_LEVEL_2				(2)
#define WATER_LEVEL_3				(3)

#define LOW_BATTERY_POWER			(650)

typedef struct {
	uint16_t	scr_addr;
	uint16_t	des_addr;
	uint8_t		type;
}  __attribute__((__packed__)) lora_header_t;

typedef struct {
	lora_header_t header;
	uint32_t data;
} __attribute__((__packed__)) lora_message_t;

typedef struct {
	uint32_t sensor_calib_value;
	uint32_t my_node_addr;
	uint32_t my_server_addr;
	uint8_t pair_state;
} flash_package_rw_t;

void lora_message_set_data(lora_message_t* msg, uint8_t* data, uint16_t len);
void lora_post_message(lora_message_t *msg);
void btn_broadcast();
void startup_init();

extern int set_lora_cfg(flash_package_rw_t*);
extern int get_lora_cfg(flash_package_rw_t*);

#if defined(APP_DBG_EN)
#define APP_DBG(fmt, ...)		console_printf(fmt, ##__VA_ARGS__)
#else
#define APP_DBG(fmt, ...)
#endif

#endif //__APP_H__
