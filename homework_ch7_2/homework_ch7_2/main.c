#include <avr/interrupt.h>
#include <util/delay.h>
#include "iseg7.h"

void msec_delay(int n);

int main()
{
	unsigned short data = 0;
	ISeg7Init();
	sei();

	while(1){
		msec_delay(1000);
		data+=10;
		ISeg7DispNum(data,10);
	}
}

void msec_delay(int n)
{
	for (; n>0; n--)
	_delay_ms(1);
}