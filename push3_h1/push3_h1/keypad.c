#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "keypad.h"

#define N_COL 4
#define KEY_OUT PORTF
#define KEY_IN PINF
#define KEY_DIR DDRF

static unsigned char key_scan(void);
static unsigned char pin = 0;

void KeyInit(){
	KEY_OUT = 0xF0;
	KEY_DIR = 0x0F;
}

unsigned char KeyInput(void){
	unsigned char in, in1;

	in = key_scan();
	while(1){
		_delay_ms(10); _delay_ms(10); _delay_ms(10);
		in1 = key_scan();
		if(in==in1) break;
		in = in1;
	}

	if(!(in & 0xF0)){
		pin = 0;
		return 0;
	}

	if(pin == in)
	return 0;

	pin = in;
	return in;
}

static unsigned char key_scan(void){
	unsigned char out, i, in;

	out = 0x01;
	for (i = 0; i < N_COL; i++) {
		KEY_OUT = ~out;
		asm("nop"::);

		in = (~KEY_IN) & 0xF0;

		if(in){
			in += out;
			break;
		}
		out <<= 1;
	}

	return in;
}
