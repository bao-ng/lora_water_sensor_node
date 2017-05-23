#include "../platform/msp430g2553.h"

#include "adc.h"
#include "app.h"

void adc_configure() {
	ADC10CTL1 = INCH_11;					// AVcc/2
	ADC10CTL0 = SREF_1 + ADC10SHT_2 + REFON + ADC10ON;
}

uint16_t read_power_voltage() {
	uint8_t battery_percent = 0;
	ADC10CTL0 |= ENC + ADC10SC;						// Sampling and conversion start
	while (ADC10CTL1 & ADC10BUSY);					// ADC10BUSY?
	battery_percent = ((ADC10MEM - LOW_BATTERY_POWER)*100)/373;
	return battery_percent;
}
