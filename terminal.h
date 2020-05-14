#ifndef terminal_H
#define terminal_H
#endif
/************************************************************************
Title:    tinyavrterm
Description: a small terminal server for the avr
Author:   Max Brueggemann
Hardware: any AVR with hardware UART
License:  BSD-3-Clause

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
    
************************************************************************/
#include <avr/pgmspace.h>

/* baudrate */
#define BAUD 9600L 

/* Buffer sizes */
#define txbufsize 200
#define bufsize 200

/**
 * @brief    
 *           definitions for some controllers
 */
#if defined(__AVR_ATtiny2313__)
#define UART_TRANSMIT_COMPLETE_INTERRUPT USART_TX_vect
#define UART_RECEIVE_INTERRUPT   USART_RX_vect
#define UART_TRANSMIT_INTERRUPT  USART_UDRE_vect
#define UART_STATUS   UCSRA
#define UART_CONTROL  UCSRB
#define UART_DATA     UDR
#define UART_UDRIE    UDRIE

#elif defined(__AVR_ATmega164P__)
#define UART_TRANSMIT_COMPLETE_INTERRUPT USART1_TX_vect
#define UART_RECEIVE_INTERRUPT   USART1_RX_vect
#define UART_TRANSMIT_INTERRUPT  USART1_UDRE_vect
#define UART_STATUS   UCSR1A
#define UART_CONTROL  UCSR1B
#define UART_DATA     UDR1
#define UART_UDRIE    UDRIE1
#define UCSRC UCSR1C
#define RXCIE RXCIE1
#define TXCIE TXCIE1
#define RXEN RXEN1
#define TXEN TXEN1
#define UCSZ0 UCSZ10
#define U2X U2X1
#define UBRRH UBRR1H
#define UBRRL UBRR1L

#elif defined(__AVR_ATmega168PA__)|(__AVR_ATmega88P__)|(__AVR_ATmega328P__)|(__AVR_ATmega168P__)
#define UART_TRANSMIT_COMPLETE_INTERRUPT USART_TX_vect
#define UART_RECEIVE_INTERRUPT   USART_RX_vect
#define UART_TRANSMIT_INTERRUPT  USART_UDRE_vect
#define UART_STATUS   UCSR0A
#define UART_CONTROL  UCSR0B
#define UART_DATA     UDR0
#define UART_UDRIE    UDRIE0
#define UCSRC UCSR0C
#define RXCIE RXCIE0
#define TXCIE TXCIE0
#define RXEN RXEN0
#define TXEN TXEN0
#define UCSZ0 UCSZ00
#define U2X U2X0
#define UBRRH UBRR0H
#define UBRRL UBRR0L

#elif defined(__AVR_ATmega328PB__)
#define UART_TRANSMIT_COMPLETE_INTERRUPT USART0_TX_vect
#define UART_RECEIVE_INTERRUPT   USART0_RX_vect
#define UART_TRANSMIT_INTERRUPT  USART0_UDRE_vect
#define UART_STATUS   UCSR0A
#define UART_CONTROL  UCSR0B
#define UART_DATA     UDR0
#define UART_UDRIE    UDRIE0
#define UCSRC UCSR0C
#define RXCIE RXCIE0
#define TXCIE TXCIE0
#define RXEN RXEN0
#define TXEN TXEN0
#define UCSZ0 UCSZ00
#define U2X U2X0
#define UBRRH UBRR0H
#define UBRRL UBRR0L

#elif defined(__AVR_ATmega1284P__)
#define UART_TRANSMIT_COMPLETE_INTERRUPT USART0_TX_vect
#define UART_RECEIVE_INTERRUPT   USART0_RX_vect
#define UART_TRANSMIT_INTERRUPT  USART0_UDRE_vect
#define UART_STATUS   UCSR0A
#define UART_CONTROL  UCSR0B
#define UART_DATA     UDR0
#define UART_UDRIE    UDRIE0
#define UCSRC UCSR0C
#define RXCIE RXCIE0
#define TXCIE TXCIE0
#define RXEN RXEN0
#define TXEN TXEN0
#define UCSZ0 UCSZ00
#define U2X U2X0
#define UBRRH UBRR0H
#define UBRRL UBRR0L

#elif defined(__AVR_ATtiny441__)
#define UART_TRANSMIT_COMPLETE_INTERRUPT USART0_TX_vect
#define UART_RECEIVE_INTERRUPT   USART0_RX_vect
#define UART_TRANSMIT_INTERRUPT  USART0_UDRE_vect
#define UART_STATUS   UCSR0A
#define UART_CONTROL  UCSR0B
#define UART_DATA     UDR0
#define UART_UDRIE    UDRIE0
#define UCSRC UCSR0C
#define RXCIE RXCIE0
#define TXCIE TXCIE0
#define RXEN RXEN0
#define TXEN TXEN0
#define UCSZ0 UCSZ00
#define U2X U2X0
#define UBRRH UBRR0H
#define UBRRL UBRR0L

#elif defined(__AVR_ATmega8__)|| defined(__AVR_ATmega16__) || defined(__AVR_ATmega32__) || defined(__AVR_ATmega323__)
#define UART_TRANSMIT_COMPLETE_INTERRUPT USART_TXC_vect
#define UART_RECEIVE_INTERRUPT   USART_RXC_vect
#define UART_TRANSMIT_INTERRUPT  USART_UDRE_vect
#define UART_STATUS   UCSRA
#define UART_CONTROL  UCSRB
#define UART_DATA     UDR
#define UART_UDRIE    UDRIE

#else
#error "no definition available"
#endif

#ifndef F_CPU
#error " F_CPU not defined "
#else
   #define UBRR (F_CPU / 8 / BAUD ) -1 
#endif /* F_CPU */

/**
* @brief    Configures the UART. Initializes some stuff. Call this function only once.
*/
extern void termInit(const __flash char * const __flash * list, uint8_t amount);

extern uint8_t termPoll(void);

extern void termReturn(void);

extern void termPutByte(uint8_t data);

extern uint8_t termCmd;

extern void termPutFlash(const __flash char *str); 