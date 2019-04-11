#include <avr/io.h>
#include <util/delay.h>

#define DEBOUNCING_DELAY 20

void A_dot_matrix(int a);
void msec_delay(int n);

int i=0;
unsigned char pattern[8]
={0xE7, 0xDB, 0xDB, 0xBD, 0x81, 0xBD, 0x7E, 0x7E};

int main(void)
{
	PORTD = 0x01 << i;
	PORTA = pattern[i];

	DDRA=0xFF;
	DDRD=0xFF;

	while(1)
	{
		A_dot_matrix(8);// 8이상의 숫자는 모두 표시한다.
		A_dot_matrix(7);
		A_dot_matrix(6);
		A_dot_matrix(5);
		A_dot_matrix(4);
		A_dot_matrix(3);
		A_dot_matrix(2);
		A_dot_matrix(1);
		A_dot_matrix(0);
	}
}

void msec_delay(int n)
{
	for(; n>0; n--)
	_delay_ms(1);
}

void A_dot_matrix(int a)
{
	int n=0;

	while(n<1000)
	{
		msec_delay(1);
		if(++i==8) i=0;
		if(i!=a){
			PORTD = 0x01 << i;
			PORTA = pattern[i];
		}

		n++;
	}
}