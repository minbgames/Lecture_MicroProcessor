#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "keypad.h"

int main()
{
	DDRB |= (1<<DDB4);

	TCCR0=(1<<WGM01)|(1<<WGM00)|(2<<COM00);

	EICRA = (2<<ISC10)|(2<<ISC00);

	sei();

	TCCR0 |=(0x02<<CS00);

	unsigned char keypad;
	static volatile unsigned char duty = 125;

	while(1){
		keypad = KeypadInput_Press();
		switch (keypad) {
			case SW1:
			duty +=5;
			break;
			case SW2:
			duty -=5;
			break;
		}
		if(duty >= 250) duty=250;
		if(duty <= 5) duty=5;
		OCR0 = duty;
	}
}



