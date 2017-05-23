#ifndef __FLASH_H__
#define __FLASH_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
//*********
 extern void flash_init();
 extern uint8_t flash_read(uint32_t address, uint8_t* pbuf, uint32_t len);
 extern uint8_t flash_write(uint32_t address, uint8_t* pbuf, uint32_t len);
 extern uint8_t flash_erase_sector(uint32_t address);
 extern void set_cfg(uint32_t address, uint8_t *pbuf, uint16_t *argv, uint8_t set_value,uint8_t len);
//**********

#ifdef __cplusplus
}
#endif

#endif //__FLASH_H__
