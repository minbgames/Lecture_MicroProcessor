#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "iseg7.h"
#include "keypad.h"

int main(){
	unsigned char key;
	unsigned short data = 0;

	ISeg7Init();
	KeyInit();
	sei();
	ISeg7DispNum(data, 10);

	while(1){
		key = KeyInput();
		switch(key){
			case SW1:
				data +=1;	break;
			case SW2:
				data +=2;	break;
			case SW3:
				data +=3;	break;
			default:
				break;
		}
		ISeg7DispNum(data, 10);
	}
}