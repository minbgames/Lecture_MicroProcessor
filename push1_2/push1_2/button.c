#include <avr/io.h>
#include <util/delay.h>
#include "button.h"

#define BTN_INPORT PIND
#define BTN_DDR DDRD
static volatile unsigned char psw=0;

void BtnInit(void){
	BTN_DDR &= ~(BTN_MASK);
	PORTD = 0x07;
}

unsigned char BtnInput(void)
{
	unsigned char sw, sw1;

	sw = ~BTN_INPORT & BTN_MASK;
	while(1){
		_delay_ms(10);  _delay_ms(10);  _delay_ms(10);
		sw1 = ~BTN_INPORT & BTN_MASK;
		if(sw == sw1) break;
		sw = sw1;
	}
	if(sw == psw) return 0;

	psw = sw;
	return sw;
}