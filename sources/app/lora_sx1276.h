#ifndef __LORA_SX1276_H__
#define __LORA_SX1276_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

///////////////////////////////////////////////// LoRa mode //////////////////////////////////////////////////
//Error Coding rate (CR)setting
#define CR_4_5
//#define CR_4_6
//#define CR_4_7
//#define CR_4_8

#ifdef	CR_4_5
#define	CR		0x01						// 4/5
#else
#ifdef	CR_4_6
#define	CR		0x02						// 4/6
#else
#ifdef	CR_4_7
#define	CR		0x03						// 4/7
#else
#ifdef	CR_4_8
#define	CR		 0x04						// 4/8
#endif
#endif
#endif
#endif

//CRC Enable
#define CRC_EN

#ifdef  CRC_EN
#define MY_CRC   0x01						//CRC Enable
#else
#define MY_CRC   0x00
#endif

// LORA board definitions
#define LR_RegOpMode								0x01
#define LR_RegFrMsb									0x06
#define LR_RegPaConfig								0x09
#define LR_RegOcp									0x0B
#define LR_RegLna									0x0C
#define LR_RegFifoAddrPtr							0x0D
#define LR_RegFifoTxBaseAddr						0x0E
#define LR_RegFifoRxBaseAddr						0x0F
#define LR_RegFifoRxCurrentaddr						0x10
#define LR_RegIrqFlagsMask							0x11
#define LR_RegIrqFlags								0x12
#define LR_RegRxNbBytes								0x13
#define LR_RegModemStat								0x18
#define LR_RegModemConfig1							0x1D
#define LR_RegModemConfig2							0x1E
#define LR_RegSymbTimeoutLsb						0x1F
#define LR_RegPreambleMsb							0x20
#define LR_RegPreambleLsb							0x21
#define LR_RegPayloadLength							0x22
#define LR_RegHopPeriod								0x24

#define REG_LR_DIOMAPPING1							0x40
#define REG_LR_DIOMAPPING2							0x41
#define REG_LR_VERSION								0x42
#define REG_LR_PADAC								0x4D

extern const uint8_t sx1276_freq_tbl[1][3];
extern const uint8_t sx1276_power_tbl[];
extern const uint8_t sx1276_spread_factor_tbl[] ;
extern const uint8_t sx1276_lora_bw_tbl[];

extern uint8_t Freq_Sel, Power_Sel, Lora_Rate_Sel, BandWide_Sel, s_factor;

extern void sx1276_write_buffer(uint8_t addr, uint8_t *buffer, uint8_t size) ;
extern void sx1276_write(uint8_t addr, uint8_t data) ;
extern void sx1276_sleep() ;
extern void sx1276_standby() ;
extern void sx1276_read_buffer(uint8_t addr, uint8_t *buffer, uint8_t size) ;
extern void sx1276_read(uint8_t addr, uint8_t *data);
extern void sx1276_entry_lora();
extern void sx1276_config();
extern void sx1276_lora_clear_irq();
extern void sx1276_lora_entry_rx();
extern void sx1276_lora_entry_tx();

#ifdef __cplusplus
}
#endif

#endif //__LORA_SX1276_H__
