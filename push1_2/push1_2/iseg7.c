#include <avr/io.h>
#include <util/delay.h>
#include "iseg7.h"

#define N_BUF 5
#define N_SEGMENT 4
#define SSEG_CONT PORTB
#define DDR_CONT DDRB
#define SSEG_DATA PORTE
#define DDR_DATA DDRE

#define CONT_MASK ((1<<PB3)|(1<<PB2)|(1<<PB1)|(1<<PB0))

#define OUTPUT_VALUE (1<<PB0)

static unsigned char SegTable[17]
={0x3f, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x58, 0x5E, 0x79, 0x71, 0x00};

static unsigned char cnum[N_BUF]={16};

void Seg7Init()
{
	SSEG_CONT|=CONT_MASK;
	DDR_CONT|=CONT_MASK;
	DDR_DATA = 0xFF;
}

void Seg7DispNum(unsigned short num, unsigned short radix)
{
	int j;
	for (j = 1; j < N_BUF; j++) cnum[j] =16;

	j=0;
	do{
		cnum[j++]=num%radix;
		num/=radix;
	}while(num);

	for (j = 0; j < N_SEGMENT; j++) {
		SSEG_CONT = ( SSEG_CONT & ~CONT_MASK)|(~(OUTPUT_VALUE<<j)&CONT_MASK);
		SSEG_DATA = ~SegTable[cnum[j]];
		_delay_ms(5);
	}
}
