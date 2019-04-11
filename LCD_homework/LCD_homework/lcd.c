#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"

// LCD 포트 주소

#define LCD_PORT PORTA
#define LCD_DDR DDRA

// 내부 함수
static void check_busy(void);
static void write_command(char command);
static void write_data(char ch);

void LcdInit(void)
{
	LCD_DDR = 0xFF;
	_delay_ms(15);
	write_command(0x30);
	_delay_ms(5);
	write_command(0x30);
	_delay_ms(1);
	write_command(0x32);

	LcdCommand(FUNSET);
	LcdCommand(DISP_OFF);
	LcdCommand(ALLCLR);
	LcdCommand(ENTMOD);
	LcdCommand(DISP_ON);
}

void LcdCommand(char command)
{
	check_busy();
	write_command(command);
	if(command==ALLCLR || command==HOME)
	_delay_ms(2);
}

void LcdPutchar(char ch)
{
	check_busy();
	write_data(ch);
}

void LcdPuts(char* str)
{
	while(*str)
	{
		LcdPutchar(*str);
		str++;
	}
}

void LcdMove(char line, char pos)
{
	pos = (line << 6) + pos;
	pos |=0x80; //비트 7을 세트한다.

	LcdCommand(pos);
}

void LcdNewchar(char ch, char font[])
{
	int i;
	ch <<= 3;
	ch |= 0x40;		//비트 6을 세트(CGRAM 주소 설정)

	LcdCommand(ch);	//CGRAM 주소 설정(LcdPutchar()로 쓰는 글꼴을 CGRAM에 저장)

	for(i=0; i<8; i++)
	LcdPutchar(font[i]);
}

static void write_command(char command)
{
	char temp;

	temp = (command & 0xF0) | 0x04;
	LCD_PORT = temp;
	LCD_PORT = temp & ~0x04;		//E = 0;

	temp = (command <<4) | 0x04;
	LCD_PORT = temp;
	LCD_PORT = temp & ~0x04;
	_delay_us(1);
}

static void write_data(char ch)
{
	unsigned char temp;

	temp = (ch & 0xF0) | 0x05;
	LCD_PORT = temp;
	LCD_PORT = temp & ~0x04;

	temp = (ch << 4) | 0x05;
	LCD_PORT = temp;
	LCD_PORT = temp & ~0x04;
}

static void check_busy()
{
	_delay_us(10);	_delay_us(10);	_delay_us(10);
	_delay_us(10);	_delay_us(10);
}
