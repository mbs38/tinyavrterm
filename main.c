#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "terminal.h"

#define FSTR(X) ((const __flash char[]) { X } )

const __flash char * const __flash myCommandList [] = 
{
    FSTR ("help\x00"), FSTR ("cmd1\x00"), FSTR ("cmd2\x00")
};

const __flash char * const __flash messages[] = 
{
    FSTR ("Welcome to tinyavrterm!\x00"), FSTR ("Command not found.\x00"), FSTR ("Boom!")
};

int main(void)
{
	termInit(myCommandList,sizeof(myCommandList)/2); //initialize uart and pass command list
	sei(); //enable interrupts globally
	while(1) {
		
		if(termPoll()) {
			termPutByte(0x0A);
			termPutByte(0x0D); 
			switch(termCmd) {
				case 0: {
					// do sth 
					termPutFlash(messages[0]);
				}
				break;

				case 1: {
					// do sth else
					termPutFlash(messages[2]);
				}
				break;

				case 2: {
					// do sth else..
					termPutFlash(messages[2]);
				}
				break;

				default: {
					// display command not found message
					termPutFlash(messages[1]);
				}
				break;
			}
			termReturn();
		}
	}
}
