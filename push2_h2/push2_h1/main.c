#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "button.h"

int main()
{
	DDRB |= (1<<DDB4);

	TCCR0=(1<<WGM01)|(1<<WGM00)|(2<<COM00);

	EICRA = (2<<ISC10)|(2<<ISC00);

	sei();

	TCCR0 |=(0x02<<CS00);

	unsigned char btn;
	unsigned char pressed;
	static volatile unsigned char duty = 125;

	while(1){
		btn = BtnInput_Press(&pressed);
		switch (btn) {
			case BTN_SW0:
			duty +=5;
			break;
			case BTN_SW1:
			duty -=5;
			break;
		}
		if(duty >= 250) duty=250;
		if(duty <= 5) duty=5;
		OCR0 = duty;
	}
}


