// Copyright (C) 2014  xorly

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.


#ifndef __UART_LIB  //not yet included

	#include <inttypes.h>
	#include "config.h"
	#include "misc.h"

	#ifndef FOSC
	#	define FOSC 16000000UL
	#endif

	#ifndef BAUD
	#	define BAUD 9600UL
	#endif

	#define ubrr_val (FOSC/(16*BAUD)-1)

	#define __NOP __asm__ volatile("NOP")



	#if (defined UART_RX || defined UART_TX)
	#	ifdef UART_RX
			unsigned char uart_getc( void );
			void uart_flush(void);
	#	endif

	#	ifdef UART_TX
			void uart_putc(char data );
			void uart_puts(char *str);
			//void uart_num(int32_t num,uint8_t base, uint8_t min_length);
	#		include "misc.h" //tools for number conversion to array of chars
	#		define uart_num(V,M) uart_puts(itoa(V,M))
	#	endif
	#else
	#	warning "not RX nor TX enabled! Do not import or define at least one of: UART_RX, UART_RX."
	#endif

	void uart_init(void);

	#define __UART_LIB
#endif
