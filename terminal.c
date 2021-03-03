/*************************************************************************
Title:    tinyavrterm
Author:   Max Brueggemann
Hardware: any AVR with hardware UART
License:  BSD-3-Clause
          
DESCRIPTION:
    Refer to the header file terminal.h.
    
USAGE:
    Refer to the header file terminal.h.
                    
LICENSE:

Copyright 2020 Max Brueggemann, www.maxbrueggemann.de

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software without
specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
THE POSSIBILITY OF SUCH DAMAGE.
                        
*************************************************************************/

#include <avr/io.h>
#include "terminal.h"
#include <avr/interrupt.h>

volatile uint8_t termCmdAmount = 0;

volatile uint8_t parseFlag = 0;

void requestParse(void) { 
	UART_CONTROL &= ~(1<<RXCIE);
	parseFlag=1;
} 

const __flash char * const __flash * termCmdList;

/*************** tx ringbuffer **************/
volatile uint8_t txbuf[txbufsize];
volatile uint8_t txbwpos = 0; //writeposition
volatile uint8_t txbrpos = 0; //readposition
volatile uint8_t txbdistance = 0;

#define TXBBUFFER_FULL (txbdistance==(txbufsize-1)) 
#define TXBDATA_READY (txbdistance!=0)

void limIncr(volatile uint8_t * var) {
        if (*var<(txbufsize-1)) {
                (*var)++;
        } else *var=0;
}

void limDecr(volatile uint8_t * var) {
        if (*var) {
                (*var)--;
        } else *var=txbufsize-1;
}

void termPutByte(uint8_t c) {
		if(!TXBBUFFER_FULL) {
       		txbuf[txbwpos] = c;
        	limIncr(&txbwpos);
        	limIncr(&txbdistance);
			UART_CONTROL|=(1<<UART_UDRIE);
		}
}

uint8_t txbbpop(void) {
        uint8_t out;
        out=txbuf[txbrpos];
        limIncr(&txbrpos);
        limDecr(&txbdistance);
        return out;
}

void txbbclear(void) {
        txbrpos=0;
        txbwpos=0;
        txbdistance=0;
}

/***************** rx buffer ****************/
volatile uint8_t rxbuf[bufsize];
volatile uint16_t rxpos = 0;
#define BUFFER_FULL (rxpos>=(bufsize-2)) //-2 to ensure null termination
#define DATA_READY (rxpos>0)
#define BUFFER_EMPTY (rxpos==0)
 
void rxbclear(void) {
	for (uint16_t i = 0; i < bufsize; i++)
	{
		rxbuf[i]=0x00;
	}
	rxpos=0;
}

void rxbpush(uint8_t data) {
	if (data>0x1F && data<0x7F)
	{
		rxbuf[rxpos++]=data;
	}
}
/********************************************/

uint16_t getStrLen(volatile uint8_t * str) {
	uint16_t i = 0;
	while(str[i]!=0) {
		i++;
	}
	return i;
}

uint16_t getStrLenFlash(const __flash char *str) {
	uint16_t i = 0;
	while(str[i]!=0) {
		i++;
	}
	return i;
}

void termPutFlash(const __flash char *str) {
	uint16_t len = 0;
	len=getStrLenFlash(str);
	for (uint16_t i = 0; i < len; i++)
	{
		termPutByte(str[i]);
	}
}

ISR(UART_RECEIVE_INTERRUPT)
{
	unsigned char data;
	data = UART_DATA;
	if((data>0x1F) && (data<0x7F) && (!parseFlag)) termPutByte(data); // no typing while busy and don't repeat non ascii chars
	if ((data == 0x0A || data == 0x0D)) //end of cmd
	{	
		requestParse();
	} else {
		if (data == 0x03 || data == 0x1B || BUFFER_FULL) // return on escape, end of text and buffer overflow
		{
			rxbclear(); //clear buffer
			requestParse();
		} else {
			if(BUFFER_EMPTY) {
				rxbclear(); // clear buffer to ensure safe parseing
				if (data != 0x20) // skip leading spaces
				{
					rxbpush(data);
				}
			} else {
				rxbpush(data);
			}
		}
	}
	
}

uint8_t compareStr(const __flash char *stra, uint16_t lena, volatile uint8_t * strb, uint16_t lenb) {
	if (lena!=lenb)
	{
		return 0;
	} else {
		for (uint16_t i = 0; i < lena; i++)
		{
			if (stra[i]!=strb[i])
			{
				return 0;
			}
		}
	}
	return 1;
}

uint16_t findSpacePos(volatile uint8_t * buffer, uint16_t scope) {
	for (uint16_t i = 0; i < scope; i++)
	{
		if (buffer[i]==0x20)
		{
			return i;
		}
	}
	return 0xFFFF;
}

uint8_t findCmd(void) {
	uint16_t cmdend=0;
	uint8_t cmd = 0xFF;
	cmdend=findSpacePos(rxbuf, bufsize);
	if(cmdend==0xFFFF) { //no space found in buffer = > 0 args passed
		cmdend=getStrLen(rxbuf); //find end of string instead
	} 
	if (cmdend<bufsize) {
		uint8_t i = 0;
		while(i < termCmdAmount)
		{
			if(compareStr(termCmdList[i], getStrLenFlash(termCmdList[i]), rxbuf, cmdend)) {
				cmd = i;
				break;
			}
			i++;
		}
	}
	return cmd;
}

uint8_t termCmd = 0xFF;

void termReturn(void) {
	termPutByte(0x0A);
	termPutByte(0x0D);
	termPutByte('$');
	termPutByte(0x20);
	rxbclear();
	termCmd=0xFF;
	parseFlag=0;
	UART_CONTROL |= (1<<RXCIE);
}

uint8_t termPoll(void) {
	if (parseFlag)
	{	
		if (BUFFER_EMPTY) //no bytes in buffer
		{
			termReturn();
		} else {
			termCmd = findCmd();
			return 1;
		}
	}
	return 0;
}

ISR(UART_TRANSMIT_INTERRUPT)
{
	UART_DATA=txbbpop();
	if (!TXBDATA_READY) {
		UART_CONTROL&=~(1<<UART_UDRIE);
	}
}

void termInit(const __flash char * const __flash * list, uint8_t amount)
{
	termCmdList = list;
	termCmdAmount=amount;
	UBRRH = (unsigned char)((UBRR) >> 8);
	UBRRL = (unsigned char) UBRR;
	UART_STATUS = (1<<U2X); //double speed mode.
	#ifdef URSEL   // in case UBRRH and UCSRC have the same location
		UCSRC = (1<<URSEL)|(3<<UCSZ0); //Frame Size
	#else
   		UCSRC = (3<<UCSZ0); //Frame Size
	#endif
	UART_CONTROL = (1<<RXCIE)|(1<<RXEN)|(1<<TXEN); // enable receiver+transmitter and receive complete interrupt
	rxbclear();
	txbbclear();
}
