#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "iseg7.h"
#include "button.h"

int main()
{
	unsigned char btn;
	unsigned char pressed;
	unsigned short data = 0;

	ISeg7Init();
	BtnInit();
	sei();
	ISeg7DispNum(data,10);

	while(1){
		btn = BtnInput_Press(&pressed);
		switch (btn) {
			case BTN_SW0:
			data +=1;
			break;
			case BTN_SW1:
			data +=2;
			break;
			case BTN_SW2:
			data +=3;
			break;
		}
		ISeg7DispNum(data, 10);
	}
}
