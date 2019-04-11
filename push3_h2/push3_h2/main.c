#include <avr/io.h>
#include <avr/interrupt.h>


static unsigned char pattern[8]
={0xFE,0xFD,0xFB,0xF7,0xEF,0xDF,0xBF,0x7F};

ISR(TIMER2_OVF_vect)
{
	static int index = 0;
	static char n_enter = 0;

	TCNT0 = 100;
	n_enter++;
	if(n_enter == 20)
	{
		n_enter = 0;
		PORTA = pattern[index++];
		if(index==8) index=0;
	}
}

int main(void)
{
	DDRA = 0xFF;
	PORTA = 0xFF;
	DDRD = 0xFF;
	PORTD = 0xFF;

	TCCR2 = 0x00;
	TCNT2 = 100;

	TIMSK = (1 << TOIE2);
	sei();

	TCCR2 |=(5 << CS20);
	while(1);
}