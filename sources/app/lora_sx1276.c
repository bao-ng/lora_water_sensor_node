#include "lora_sx1276.h"

#include "../platform/msp430g2553.h"
#include "../platform/spi/spi.h"

const uint8_t sx1276_freq_tbl[1][3] = {
	{0x6C, 0x80, 0x00},		//434MHz
};

const uint8_t sx1276_power_tbl[4] = {
	0xFF,	//20dbm
	0xFC,	//17dbm
	0xF9,	//14dbm
	0xF6,	//11dbm
};

const uint8_t sx1276_spread_factor_tbl[7] = {
	6,7,8,9,10,11,12
};

const uint8_t sx1276_lora_bw_tbl[10] = {
	//7.8KHz,10.4KHz,15.6KHz,20.8KHz,31.2KHz,41.7KHz,62.5KHz,125KHz,250KHz,500KHz
	0,1,2,3,4,5,6,7,8,9
};

uint8_t				Freq_Sel, Power_Sel, Lora_Rate_Sel, BandWide_Sel, s_factor = 0;

static void sx1276_select();
static void sx1276_deselect();

void sx1276_select() {
	P1OUT &= ~BIT4;
}

void sx1276_deselect() {
	P1OUT |= BIT4;
}

void sx1276_write_buffer(uint8_t addr, uint8_t *buffer, uint8_t size) {
	uint8_t i;
	sx1276_select();

	spi_send(addr | 0x80);

	for (i = 0; i < size; i++) {
		spi_send(buffer[i]);
	}

	sx1276_deselect();
}

void sx1276_write(uint8_t addr, uint8_t data) {
	sx1276_select();
	spi_send(addr | 0x80);
	spi_send(data);
	sx1276_deselect();
}

void sx1276_sleep(void) {
	sx1276_write(LR_RegOpMode, 0x08);					//Sleep	+ Low Frequency Mode
}

void sx1276_standby(void) {
	sx1276_write(LR_RegOpMode, 0x09);					//Standby + Low Frequency Mode
}

void sx1276_read_buffer(uint8_t addr, uint8_t *buffer, uint8_t size) {
	uint8_t i;

	sx1276_select();

	spi_send(addr & 0x7F);

	for (i = 0; i < size; i++) {
		buffer[i] = spi_send(0);
	}

	sx1276_deselect();
}

void sx1276_read(uint8_t addr, uint8_t *data) {
	sx1276_select();
	spi_send(addr & 0x7F);
	*data = spi_send(0);
	sx1276_deselect();
}

void sx1276_entry_lora() {
	sx1276_write(LR_RegOpMode, 0x88);				//Low Frequency Mode
}

void sx1276_config(void) {
	uint8_t tmp;

	P1DIR |= BIT4;
	P1OUT |= BIT4;

	sx1276_sleep();

	sx1276_entry_lora();
	sx1276_write_buffer(LR_RegFrMsb, (uint8_t *) sx1276_freq_tbl[Freq_Sel], 3);

	sx1276_write(LR_RegPaConfig, sx1276_power_tbl[Power_Sel]);

	sx1276_write(LR_RegOcp, 0x0B);
	sx1276_write(LR_RegLna, 0x23);

	if (sx1276_spread_factor_tbl[Lora_Rate_Sel] == 6) {		//SFactor=6
		sx1276_write(LR_RegModemConfig1, ((sx1276_lora_bw_tbl[BandWide_Sel]<<4)+(CR<<1)+0x01));
		sx1276_write(LR_RegModemConfig2, ((sx1276_spread_factor_tbl[Lora_Rate_Sel]<<4)+(MY_CRC<<2)+0x03));

		sx1276_read(0x31, (uint8_t *) &tmp);
		tmp &= 0xF8;
		tmp |= 0x05;
		sx1276_write(0x31, tmp);
		sx1276_write(0x37, 0x0C);

		s_factor = 6;

	} else {
		sx1276_write(LR_RegModemConfig1, ((sx1276_lora_bw_tbl[BandWide_Sel]<<4)+(CR<<1)+0x00));
		sx1276_write(LR_RegModemConfig2, ((sx1276_spread_factor_tbl[Lora_Rate_Sel]<<4)+(MY_CRC<<2)+0x03));
	}

	sx1276_write(LR_RegSymbTimeoutLsb, 0xFF);

	sx1276_write(LR_RegPreambleMsb, 0x00);
	sx1276_write(LR_RegPreambleLsb, 12);

	sx1276_write(REG_LR_DIOMAPPING2, 0x01);

	sx1276_standby();							//Entry standby mode
}

void sx1276_lora_clear_irq(void) {
	sx1276_write(LR_RegIrqFlags, 0xFF);
}

void sx1276_lora_entry_rx(void) {
	uint8_t addr;
	sx1276_config();								//setting base parameter
	sx1276_write(REG_LR_PADAC, 0x84);
	sx1276_write(LR_RegHopPeriod, 0xFF);
	sx1276_write(REG_LR_DIOMAPPING1, 0x01);
	sx1276_write(LR_RegIrqFlagsMask, 0x3F);
	sx1276_lora_clear_irq();
	sx1276_write(LR_RegPayloadLength, 21);
	sx1276_read(LR_RegFifoRxBaseAddr, (uint8_t *) &addr);
	sx1276_write(LR_RegFifoAddrPtr, addr);
	sx1276_write(LR_RegOpMode, 0x8d);
}

void sx1276_lora_entry_tx(void) {
    uint8_t addr;
	sx1276_config();								//setting base parameter
	sx1276_write(REG_LR_PADAC, 0x87);
	sx1276_write(LR_RegHopPeriod, 0x00);
	sx1276_write(REG_LR_DIOMAPPING1, 0x01);
	sx1276_lora_clear_irq();
	sx1276_write(LR_RegIrqFlagsMask, 0xF7);
	sx1276_write(LR_RegPayloadLength, 21);
	sx1276_read(LR_RegFifoTxBaseAddr, (uint8_t *) &addr);
	sx1276_write(LR_RegFifoAddrPtr, addr);
}
