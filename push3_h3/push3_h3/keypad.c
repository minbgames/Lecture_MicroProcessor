#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "keypad.h"

#define N_COL 4
#define KEY_OUT PORTF
#define KEY_IN PINF
#define KEY_DIR DDRF

static unsigned char key_scan(void);
static volatile unsigned char psw=0;

void KeyInit(){
	KEY_OUT = 0xF0;
	KEY_DIR = 0x0F;
}

unsigned char KeypadInput_Press(void)
{
	static unsigned char press_cnt=0;
	unsigned char sw, sw1;

	sw = key_scan();
	while(1){
		_delay_ms(10);  _delay_ms(10);  _delay_ms(10);
		sw1 = key_scan();
		if(sw == sw1) break;
		sw = sw1;
	}
	if(sw == psw){
		press_cnt++;
		if(press_cnt > 16){
			for (int i=0; i<7; i++) _delay_ms(10);
		}
		else{
			return 0;
		}
	}
	else{
		press_cnt = 0;
	}
	psw =sw;
	return sw;
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