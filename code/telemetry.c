// Copyright (C) 2016  xorly

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

#include "telemetry.h"
#include <avr/interrupt.h>
#include <inttypes.h>
#include "uart.h"

#include "hardware.h"

#define STATE_IDLE		0
#define STATE_GOT_BOUNDS 1
#define STATE_RX_DATA	2
#define STATE_ERROR		3

#define FRAME_BOUNDS	0x7e
#define FRAME_TYPE_STD	0xfe

#define FRAME_LENGTH_MAX	(10+1)//we are expecting standard frame, nothing more (eg. sensor hub,...)


uint8_t rx_buffer[FRAME_LENGTH_MAX];
uint8_t rx_buffer_pos=0;
uint8_t state=STATE_IDLE;

uint8_t telemetry_analog_convert_value(uint8_t raw, uint8_t range)
{
	return (uint16_t)raw*range/TELEMETRY_ANALOG_MAX_RANGE;
}

void telemetry_process_frame(uint8_t raw[])
{
	switch(raw[0])
	{
		case FRAME_TYPE_STD:
			telemetry.analog_A1.raw=raw[1];
			telemetry.analog_A2.raw=raw[2];
			telemetry.rssi_RX=raw[3];
			telemetry.rssi_TX=raw[4]/2;
			telemetry.updated=1;//flag indicating fresh data
			//rest were zeros
			telemetry.enabled=1;
			break;
	}
	//notify, that we have fresh telemetry data
}

ISR(USART_RXC_vect)
{
	uint8_t data=UDR;
	sei();//reenable interrupt asap, but it has to be after clearing UDR => ISR_NOBLOCK will cause infinite recursion...
	switch(state)
	{
		case STATE_IDLE:
			if(data==FRAME_BOUNDS)
			{
				state=STATE_GOT_BOUNDS;
				rx_buffer_pos=0;
			}
			break;

		case STATE_GOT_BOUNDS:
			if(data!=FRAME_BOUNDS)//we have first data here...
			{
				rx_buffer[rx_buffer_pos++]=data;//first byte is describes frame type
				state=STATE_RX_DATA;
			}
			break;

		case STATE_RX_DATA:
			if(data!=FRAME_BOUNDS)
			{
				if(rx_buffer_pos>=FRAME_LENGTH_MAX )
				{
					state=STATE_ERROR;
					break;
				}
				else
					rx_buffer[rx_buffer_pos++]=data;
			}
			else
			{
				state=STATE_IDLE;
				//TODO: should be telemetry parse be done in IRQ handler
				telemetry_process_frame(rx_buffer);
			}
			break;

		default:
			state=STATE_ERROR;
	}

	if(state==STATE_ERROR)
	{
		//inform about it somehow
		state=STATE_IDLE;
	}
}