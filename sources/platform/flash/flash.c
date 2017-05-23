#include "../platform/msp430g2553.h"
#include "stdlib.h"
#include "flash.h"
#include"console.h"
#include "stdio.h"
void flash_init() {
	WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog timer
	if (CALBC1_1MHZ == 0xFF)					// If calibration constant erased
	{
		while(1);                               // do not load, trap CPU!!
	}
	DCOCTL  = 0;                               // Select lowest DCOx and MODx settings
	BCSCTL1 = CALBC1_1MHZ;                    // Set DCO to 1MHz
	DCOCTL  = CALDCO_1MHZ;
	FCTL2   = FWKEY + FSSEL0 + FN1;             // MCLK/3 for Flash Timing Generator                                // initialize value
}

uint8_t flash_write(uint32_t address, uint8_t* pbuf, uint32_t len) {
	uint32_t *Flash_ptr;                          // Flash pointer
	unsigned int i;

	Flash_ptr = (uint32_t *) address;              // Initialize Flash pointer
	FCTL1 = FWKEY + ERASE;                    // Set Erase bit
	FCTL3 = FWKEY;                            // Clear Lock bit
	*Flash_ptr = 0;                           // Dummy write to erase Flash segment

	FCTL1 = FWKEY + WRT;                      // Set WRT bit for write operation

	for (i=0; i < len; i++)
	{
		*Flash_ptr++ = pbuf[i];                   // Write value to flash
	}

	FCTL1 = FWKEY;                            // Clear WRT bit
	FCTL3 = FWKEY + LOCK;                     // Set LOCK bit
}

uint8_t flash_erase_sector(uint32_t address) {
	uint32_t *Flash_ptr;
	unsigned int i;

	Flash_ptr  = (uint32_t *) address;             // Initialize Flash segment C pointer
	FCTL1      = FWKEY + ERASE;                    // Set Erase bit
	FCTL3      = FWKEY;                            // Clear Lock bit
	*Flash_ptr = 0;                          // Dummy write to erase Flash segment D
	FCTL1      = FWKEY + WRT;                      // Set WRT bit for write operation

	for (i=0; i < 64; i++)
	{
		*Flash_ptr++ = 0;
	}

	FCTL1 = FWKEY;                            // Clear WRT bit
	FCTL3 = FWKEY + LOCK;                     // Set LOCK bit
	return 0;
}

uint8_t flash_read(uint32_t address, uint8_t* pbuf, uint32_t len) {
	uint32_t *Flash_ptr;                          // Flash pointer
	unsigned int i;

	Flash_ptr = (uint32_t*)address;              // Initialize Flash pointer

	for (i=0; i < len; i++) {
		while ((FCTL3 & BUSY) != 0);
		pbuf[i] = *Flash_ptr++ ;                // Write value to flash
	}

	return 0;
}

