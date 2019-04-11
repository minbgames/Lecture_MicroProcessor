#include <avr/io.h>
#include <util/delay.h>
#include "button.h"

#define BTN_INPORT PIND
#define BTN_DDR DDRD
#define PRESS_THRESHOLD_1 16
#define PRESS_THRESHOLD_2 66
static volatile unsigned char psw=0;

void BtnInit(void){
	BTN_DDR &= ~(BTN_MASK);
}

unsigned char BtnInput_Press(unsigned char* pressed)
{
	static unsigned char press_cnt=0;
	unsigned char sw, sw1;

	sw = ~BTN_INPORT & BTN_MASK;
	while(1){
		_delay_ms(10);  _delay_ms(10);  _delay_ms(10);
		sw1 = ~BTN_INPORT & BTN_MASK;
		if(sw == sw1) break;
		sw = sw1;
	}
	*pressed = 0;
	if(sw == psw){
		press_cnt++;
		if(press_cnt > PRESS_THRESHOLD_1){
			
			if(press_cnt> PRESS_THRESHOLD_2){
				*pressed = 2;
				for (int i=0; i<2; i++) _delay_ms(10);
			}
			else{
				*pressed = 1;
				for (int i=0; i<7; i++) _delay_ms(10);
			}	
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