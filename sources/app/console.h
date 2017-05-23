#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include <stdint.h>

#include "app.h"
#include "cmd_line.h"

#define CONSOLE_BUFFER_LENGHT			(16)

extern cmd_line_t lgn_cmd_table[];

typedef struct  {
	uint8_t index;
	uint8_t improgress;
	uint8_t data[CONSOLE_BUFFER_LENGHT];
} console_t;

extern console_t console;

extern void console_init();
extern void console_task();
extern void console_printf(const char* fmt, ...);

#endif //__CONSOLE_H__
