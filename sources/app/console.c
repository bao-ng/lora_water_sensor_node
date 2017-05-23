#include <stdarg.h>

#include "../platform/msp430g2553.h"

#include "console.h"

console_t console;

void xputc(uint8_t c) {
	if (c == (uint8_t)'\n') {
		while (!(IFG2&UCA0TXIFG));
		UCA0TXBUF = '\r';
	}

	while (!(IFG2&UCA0TXIFG));
	UCA0TXBUF = c;
}

void console_init() {
	P1SEL		|= BIT1 + BIT2 ;		// P1.1 = RXD, P1.2=TXD
	P1SEL2		|= BIT1 + BIT2 ;		// P1.1 = RXD, P1.2=TXD
	UCA0CTL1	|= UCSSEL_2;		// SMCLK
	UCA0BR0		= 104;				// 1MHz 9600
	UCA0BR1		= 0;				// 1MHz 9600
	UCA0MCTL	= UCBRS0;			// Modulation UCBRSx = 1
	UCA0CTL1	&= ~UCSWRST;		// **Initialize USCI state machine**
	IE2			|= UCA0RXIE;		// Enable USCI_A0 RX interrupt
}

void __attribute__ ((interrupt(USCIAB0RX_VECTOR))) USCI0RX_ISR (void) {
	uint8_t c = 0;

	c = UCA0RXBUF;

	if (console.improgress) {
		APP_DBG("\nerror:console busy\n");
	}
	else {
		if (console.index < CONSOLE_BUFFER_LENGHT - 1 || console.improgress) {
			if (c == '\r' || c == '\n') {
				console.data[console.index] = c;
				console.data[console.index + 1] = 0;

				console.index = 0;
				console.improgress = 1;
			}
			else if (c == 8) {
				if (console.index) {
					console.index--;
				}
			}
			else {
				console.data[console.index++] = c;
			}
		}
		else {
			APP_DBG("\nerror: cmd too long, cmd size: %d, try again !\n", CONSOLE_BUFFER_LENGHT);
			console.index = 0;
		}
	}
}

void console_task() {
	if (console.improgress) {
		switch (cmd_line_parser(lgn_cmd_table, console.data)) {
		case CMD_SUCCESS:
			break;

		case CMD_NOT_FOUND:
			if (console.data[0] != '\r' &&
					console.data[0] != '\n') {
				APP_DBG("cmd unknown\n");
			}
			break;

		case CMD_TOO_LONG:
			APP_DBG("cmd too long\n");
			break;

		case CMD_TBL_NOT_FOUND:
			APP_DBG("cmd table not found\n");
			break;

		default:
			APP_DBG("cmd error\n");
			break;
		}

		APP_DBG("#");

		console.improgress = 0;
	}
}

void console_printf(const char* fmt, ...) {
	va_list va_args;
	uint32_t num, digit;
	int32_t i;
	int32_t zero_padding = 0;
	int32_t format_lenght = 0;
	int32_t base;
	int32_t minus;
	int8_t num_stack[11];
	uint8_t* ps;

	va_start(va_args, fmt);

	while (*fmt) {
		switch (*fmt) {
		case '%':
			zero_padding = 0;
			if (fmt[1] == '0') {
				zero_padding = 1;
				++fmt;
			}
			format_lenght = 0;
			while (*++fmt) {
				switch (*fmt) {
				case '%':
					xputc(*fmt);
					goto next_loop;

				case 'c':
					xputc(va_arg(va_args, int32_t));
					goto next_loop;

				case 'd':
				case 'X':
				case 'x':
					minus = 0;
					num = va_arg(va_args, uint32_t);
					if (*fmt == 'd') {
						base = 10;
						if (num & (uint32_t)0x80000000) {
							num = -(int32_t)num;
							minus = 1;
						}
					} else {
						base = 16;
					}
					for (digit = 0; digit < sizeof(num_stack);) {
						num_stack[digit++] = num%base;
						num /= base;
						if (num == 0) break;
					}
					if (minus) num_stack[digit++] = 0x7F;
					if (format_lenght > digit) {
						int8_t paddingint8_t = ' ';
						format_lenght -= digit;
						if (zero_padding)
							paddingint8_t = '0';
						while (format_lenght--) {
							xputc(paddingint8_t);
						}
					}
					for (i = digit-1; i >= 0; i--) {
						if (num_stack[i] == 0x7F) {
							xputc('-');
						} else if (num_stack[i] > 9) {
							xputc(num_stack[i]-10 + 'A');
						} else {
							xputc(num_stack[i] + '0');
						}
					}
					goto next_loop;

				case 's':
					ps = va_arg(va_args, uint8_t*);
					while(*ps) {
						xputc(*ps++);
					}
					goto next_loop;

				default:
					if (*fmt >= '0' && *fmt <= '9') {
						format_lenght = format_lenght*10 + (*fmt-'0');
					} else {
						goto exit;
					}
				}

			}
			if (*fmt == 0) {
				goto exit;
			}

		default:
			xputc(*fmt);
			break;
		}
next_loop:
		fmt++;
	}
exit:
	va_end(va_args);
}
