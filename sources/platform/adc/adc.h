#ifndef __ADC_H__
#define __ADC_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
//*********
    void adc_configure();
	uint16_t read_power_voltage();
//**********

#ifdef __cplusplus
}
#endif

#endif //__SPI_H__
