#include <avr/io.h>
#include <avr/interrupt.h>
#include "iseg7.h"

#define N_BUF 5
#define N_SEGMENT 4
#define SSEG_CONT PORTD
#define DDR_CONT DDRD
#define SSEG_DATA PORTA
#define DDR_DATA DDRA

#define CONT_MASK ((1<<PD7)|(1<<PD6)|(1<<PD5)|(1<<PD4))

#define OUTPUT_VALUE (1<<PD4)

static unsigned char SegTable[17]
={0x3f, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x58, 0x5E, 0x79, 0x71, 0x00};

static unsigned char cnumber[N_BUF]
={16,16,16,16,16};
static unsigned char cstr[N_BUF]={16};

void ISeg7Init()
{
	SSEG_CONT|=CONT_MASK;
	DDR_CONT|=CONT_MASK;
	DDR_DATA = 0xFF;

	TCCR0 = (1<<WGM01);
	OCR0 = 78;
	TIMSK |=(1<<OCIE0);

	TCCR0 |=0x07;
}

void ISeg7DispNum(unsigned short num, unsigned short radix)
{
	int j;

	TIMSK &= ~(1<<OCIE0);
	for (j = 1; j < N_BUF; j++) cnumber[j] =16;

	j=0;
	do{
		cnumber[j++]=num%radix;
		num/=radix;
	}while(num);

	TIMSK |= (1<<OCIE0);
}

void ISeg7DispStr(char *pch)
{
	int j;

	TIMSK &= ~(1<<OCIE0);
	for (j = 1; j < N_BUF; j++) cstr[j] =16;

	j=0;
	do{
		if(pch[j]-'A'>=0 && pch[j]-'A'<=5){
			cstr[j]=pch[j]-'A'+10;
		}
		else if(pch[j]-'a'>=0 && pch[j]-'a'<=5){
			cstr[j]=pch[j]-'a'+10;
		}
		else if(pch[j]-'0'>=0 && pch[j]-'0'<=9){
			cstr[j]=pch[j]-'0';
		}
		else{
			cstr[j]=16;
		}
		j++;

	}while(j<4);

	TIMSK |= (1<<OCIE0);
}

ISR(TIMER0_COMP_vect)
{
	static int index = 0;
	SSEG_CONT = ( SSEG_CONT & ~CONT_MASK)|(~(OUTPUT_VALUE<<index)&CONT_MASK);
	if(test==0){
		SSEG_DATA = ~SegTable[cnumber[index]];
	}
	else if(test==1){
		SSEG_DATA = ~SegTable[cstr[3-index]];
	}

	index++;
	if(index == N_SEGMENT) index=0;
}
