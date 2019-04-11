#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"

static char font1[8]= {0x04, 0x0E, 0x15, 0x04, 0x04, 0x15, 0x0E, 0x04};
static char font2[8]= {0x03, 0x03, 0x1B, 0x1F, 0x1F, 0x1B, 0x03, 0x03};
static char font3[8]= {0x1A, 0x1A, 0x0A, 0x1B, 0x1B, 0x0A, 0x0A, 0x0A};

int main()
{
	char string[20];

	LcdInit();

	LcdNewchar(0, font1);
	LcdNewchar(1, font2);
	LcdNewchar(2, font3);

	string[0] = 8;
	string[1] = 1;
	string[2] = 2;
	string[3] = 8;
	string[4] = '\0';

	LcdMove(1,2);
	LcdPuts(string);

	while(1);
}

