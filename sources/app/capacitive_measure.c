#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>

#include "../platform/msp430g2553.h"

#include "app.h"

#define DIV_SMCLK_32768	(WDT_MDLY_32)		// SMCLK/32768
#define DIV_SMCLK_8192	(WDT_MDLY_8)		// SMCLK/8192
#define DIV_SMCLK_512	(WDT_MDLY_0_5)		// SMCLK/512
#define DIV_SMCLK_64	(WDT_MDLY_0_064)	// SMCLK/64
/* Defines WDT SMCLK interval for sensor measurements*/
#define WDT_meas_setting (DIV_SMCLK_32768)
/* Defines WDT ACLK interval for delay between measurement cycles*/
#define WDT_delay_setting (DIV_SMCLK_32768)

struct Element{									// This structure contains informations of sensor pad
	unsigned char *Pxsel;						// Pointer to Pxsel register
	unsigned char *Pxsel2;						// Pointer to Pxsel2 register
	unsigned char *Pxdir;						// Pointer to Pxdir register
	unsigned char *Pxout;						// Pointer to Pxout register
	unsigned char inputBits;					// Input bit
};

const struct Element LEVEL_1 = {
	.Pxsel = (unsigned char *)&P1SEL,
	.Pxsel2 = (unsigned char *)&P1SEL2,
	.Pxdir = (unsigned char *)&P1DIR,
	.inputBits = BIT0,
};

const struct Element LEVEL_2 = {
	.Pxsel = (unsigned char *)&P2SEL,
	.Pxsel2 = (unsigned char *)&P2SEL2,
	.Pxdir = (unsigned char *)&P2DIR,
	.inputBits = BIT4,
};

const struct Element LEVEL_3 = {
	.Pxsel = (unsigned char *)&P2SEL,
	.Pxsel2 = (unsigned char *)&P2SEL2,
	.Pxdir = (unsigned char *)&P2DIR,
	.inputBits = BIT5,
};

static uint32_t measure_count(struct Element level);

uint32_t measure_count(struct Element level) {			// Measure raw capacitance
	uint32_t meas_cnt;
	TACTL = TASSEL_3+MC_2;			// TACLK, cont mode
	TACCTL1 = CM_3+CCIS_2+CAP;		// Pos&Neg,GND,Cap

	*(level.Pxdir) &= ~level.inputBits;			// input pins
	*(level.Pxsel) &= ~level.inputBits;			// TACLK input
	*(level.Pxsel2) |= level.inputBits;

	/*Setup Gate Timer*/
	WDTCTL = WDT_meas_setting;				// WDT, ACLK, interval timer
	TACTL |= TACLR;							// Clear Timer_A TAR
	__bis_SR_register(LPM0_bits+GIE);		// Wait for WDT interrupt
	TACCTL1 ^= CCIS0;						// Create SW capture of CCR1
	meas_cnt = TACCR1;						// Save result
	WDTCTL = WDTPW + WDTHOLD;				// Stop watchdog timer
	*(level.Pxsel2) &= ~level.inputBits;

	return meas_cnt;
}

uint32_t measure_water_level(uint8_t level) {
	switch (level) {
	case WATER_LEVEL_1:
		measure_count(LEVEL_1);
		break;
	case WATER_LEVEL_2:
		measure_count(LEVEL_2);
		break;
	case WATER_LEVEL_3:
		measure_count(LEVEL_3);
		break;
	default:
		break;
	}

}
