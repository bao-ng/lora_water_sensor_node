#include <string.h>
#include <stdlib.h>
#include <app.h>
#include "../platform/flash/flash.h"
#include "../platform/msp430g2553.h"

void button_init() {
	/*button broadcast*/
	if((P2IN & BIT1) == 0) {
		__delay_cycles(1000000);

		if((P2IN & BIT1) == 0) {
			flash_erase_sector(FLASH_CFG_ADDR);
			do{}while((P2IN & BIT1) == 0);
		}
	}
}

uint8_t button_state() {
	if((P2IN & BIT1) == 0) {
		__delay_cycles(30000);
		if((P2IN & BIT1) == 0) {
			return 1;
		}
		else
			return 0;
	}
	else
		return 0;
}
