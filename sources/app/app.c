#include <string.h>
#include <stdlib.h>
#include "../platform/msp430g2553.h"
#include "../platform/spi/spi.h"
#include "../platform/flash/flash.h"
#include "../platform/adc/adc.h"
#include "../platform/button/button.h"

#include "app.h"
#include "console.h"
#include "lora_sx1276.h"
#include "capacitive_measure.h"

typedef struct {
	uint32_t calib_value;
	int32_t delta_value;
	uint32_t measure_value;
} capacitive_measure_t;

lora_message_t lora_msg;		//cfg header message
flash_package_rw_t flash_package_rw;		//cfg header write/read flash
capacitive_measure_t cap_msr;

static void package_lora_message(uint8_t _type, uint32_t data);
static void init_port_irq();
static int32_t water_sensor_detect();

uint8_t count =0;
uint16_t daily_timer = 0;

void main(void) {
	Freq_Sel		= 0x00;										//433M
	Power_Sel		= 0x00;
	Lora_Rate_Sel	= 0x06;
	BandWide_Sel	= 0x07;

	/* stop watchdog timer */
	WDTCTL = WDTPW + WDTHOLD;

	/* set clock 1Mhz */
	DCOCTL  = 0;
	BCSCTL1 = CALBC1_1MHZ;
	DCOCTL  = CALDCO_1MHZ;

	BCSCTL3 |= LFXT1S_2;						// LFXT1 = VLO
	IE1 |= WDTIE;								// enable WDT interrupt

	BCSCTL2 &= ~(DIVS_0);
	/*config Led*/
	P2DIR |= BIT3;
	P2OUT |= (BIT3);

	/* configure timer */
#if defined(RELEASE)
	TA1CCR0  = 50000;
#else
	TA1CCR0  = 4000;
#endif

	TA1CTL = TASSEL_1 + MC_1 + ID_3;				// ACLK, upmode, clk/8
	spi_initialize();

	/* init console */
	console_init();

	/*init flash*/
	flash_init();

	/*init button*/
	button_init();

	/*led init*/
	P2DIR |= BIT3;

	/*init lora*/
	sx1276_config();

	/*adc config*/
	adc_configure();

	/* read flash*/
	startup_init();

	/* interrupts enabled */
	__enable_interrupt();

	sx1276_lora_entry_rx();

	while(1) {
		console_task();
		switch (flash_package_rw.pair_state) {
		case LORA_PAIRED_STATE: {
			TA1CCTL0 = CCIE;
			P2OUT |= (BIT3);
			__bis_SR_register(LPM3_bits + GIE);
		}
			break;

		case LORA_CONFIG_STATE :{
			init_port_irq();
			btn_broadcast();
			P2OUT &= ~(BIT3);
		}
			break;

		default:
			break;
		}
	}
}

void __attribute__ ((interrupt(PORT1_VECTOR))) Port_1 (void) {
	uint8_t addr;
	uint8_t packet_size;
	lora_message_t rcv_msg;

	P1IFG &= ~BIT3; /* clear interrupt flag */

	sx1276_read(LR_RegFifoRxCurrentaddr, (uint8_t *) &addr);
	sx1276_write(LR_RegFifoAddrPtr, addr);

	if (sx1276_spread_factor_tbl[Lora_Rate_Sel] == 6) {
		packet_size = 21;
	}
	else {
		sx1276_read(LR_RegRxNbBytes, &packet_size);
	}
	sx1276_read_buffer(0x00, (uint8_t*)&rcv_msg, sizeof(lora_message_t));
	sx1276_lora_clear_irq();

	if(rcv_msg.header.scr_addr==lora_msg.header.des_addr) {

		switch (rcv_msg.data) {
		case (SERVER_CONFIRM): {
			flash_read(FLASH_CFG_ADDR, (uint8_t*)&flash_package_rw, sizeof(flash_package_rw_t));
			flash_package_rw.pair_state=1;
			flash_write(FLASH_CFG_ADDR, (uint8_t*)&flash_package_rw, sizeof(flash_package_rw_t));
			P2OUT |= (BIT3);
		}
			break;
		default:
			break;
		}
	}
}

void __attribute__ ((interrupt(TIMER1_A0_VECTOR))) Timer_A0 (void) {
	__bic_SR_register_on_exit(LPM3_bits);
	int32_t current_water_level;
	current_water_level = water_sensor_detect();

	//	if(daily_timer ++ > ONE_DAY) {
	//		uint8_t battery_voltage = read_power_voltage();
	//		package_lora_message(TYPE_KEEP_ALIVE,battery_voltage);
	//		daily_timer = 0;
	//	}

	if(current_water_level > WATER_LEVEL_HIGH) {
		if(count < 10) {
			package_lora_message(TYPE_REPORT, 500);
			count ++;
		}
	}

	else {
		if(count > 0) {
			package_lora_message(TYPE_REPORT, 2500);
			count --;
		}
	}
}

void __attribute__ ((interrupt(ADC10_VECTOR ))) ADC10_ISR (void) {
	__bic_SR_register_on_exit(CPUOFF);        // Clear CPUOF
}

void __attribute__ ((interrupt(WDT_VECTOR))) watchdog_timer (void) {
	TA0CCTL1 ^= CCIS0;
	__bic_SR_register_on_exit(LPM3_bits);     // Exit LPM3 on reti
}

void lora_post_message(lora_message_t *msg) {
	uint8_t data2 = 0;
	sx1276_lora_entry_tx();
	sx1276_write_buffer(0x00, (uint8_t *)msg, sizeof(lora_message_t));
	sx1276_write(LR_RegOpMode, 0x8b);

	while (!(data2 & 0x08)) {
		sx1276_read(LR_RegIrqFlags, (uint8_t *) &data2);
	}

	sx1276_lora_clear_irq();
	sx1276_standby();
	sx1276_lora_entry_rx();
}

void btn_broadcast() {
	if(button_state()) {
		__delay_cycles(4000000);
		lora_msg.header.type = TYPE_CFG;
		lora_msg.data = 0;
		lora_post_message(&lora_msg);
	}
}

int set_lora_cfg(flash_package_rw_t* cfg) {
	flash_write(FLASH_CFG_ADDR, cfg, sizeof(flash_package_rw_t));
	return 0;
}

int get_lora_cfg(flash_package_rw_t* cfg) {
	flash_read(FLASH_CFG_ADDR, cfg, sizeof(flash_package_rw_t));
	return 0;
}

void startup_init() {
	flash_read(FLASH_CFG_ADDR, &flash_package_rw, sizeof(flash_package_rw_t));

	lora_msg.header.scr_addr=flash_package_rw.my_node_addr;
	lora_msg.header.des_addr=flash_package_rw.my_server_addr;
	cap_msr.calib_value = flash_package_rw.sensor_calib_value;

	APP_DBG("\n***\twater sensor node\t***\n");
	APP_DBG("scr_addr: %d | des_addr: %d | pair_state: %d | calib_value: %d \n",
			(uint32_t)flash_package_rw.my_node_addr,
			(uint32_t)flash_package_rw.my_server_addr,
			(uint32_t)flash_package_rw.pair_state,
			(uint32_t)flash_package_rw.sensor_calib_value);
}

void package_lora_message(uint8_t _type, uint32_t data) {
	lora_msg.header.type = _type;
	lora_msg.data = data;
	lora_post_message(&lora_msg);
}

void init_port_irq() {
	/*init port interrupts*/
	P1IE  |=  BIT3;							// P1.3 interrupt enabled
	P1IES &= ~BIT3;							// P1.3 Lo-Hi edge
	P1REN |= BIT3;							// Enable Pull Up on SW2 (P1.3)
	P1OUT |= BIT3;
	P1IFG &= ~BIT3;							// P1.3 IFG cleared
}

int32_t water_sensor_detect() {
	cap_msr.measure_value = measure_water_level(WATER_LEVEL_1);
	cap_msr.delta_value = abs(cap_msr.calib_value - cap_msr.measure_value);
	return cap_msr.delta_value;
}


