#include <avr/interrupt.h>
#include <util/delay.h>
#include "iseg7.h"

void msec_delay(int n);

int main()
{
	unsigned short data = 123;
	char *pch = "Ca#023";
	ISeg7Init();
	sei();

	test=1;

	while(1){
		if(test==1){
			ISeg7DispNum(data,10);
			test=0;
		}
		else if(test==0){
			ISeg7DispStr(pch);
			test=1;
		}
		msec_delay(1000);
	}
}

void msec_delay(int n)
{
	for (; n>0; n--)
	_delay_ms(1);
}