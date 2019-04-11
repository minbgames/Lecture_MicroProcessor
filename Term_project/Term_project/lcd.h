
/************************************************************************/
/*  LCD의 코드는 예제를 그대로 이용했기 때문에 따로 주석을 달지 않았다. */
/************************************************************************/

#ifndef __LCD_H__
#define __LCD_H__

// LCD 제어 명령
#define	ALLCLR			0x01
#define HOME			0x02
#define LN21			0xc0

#define ENTMOD			0x06
#define FUNSET			0x28
#define DISP_ON			0x0c
#define DISP_OFF		0x08
#define CURSOR_ON		0x0e
#define CURSOR_OFF		0x0c
#define CURSOR_LSHIFT	0x10
#define CURSOR_RSHIFT	0x14
#define DISP_LSHIFT		0x18
#define DISP_RSHIFT		0x1c

//LCD 제어 모듈 함수
void LcdInit(void);
void LcdCommand(char command);
void LcdMove(char line, char pos);
void LcdPutchar(char ch);
void LcdPuts(char* str);
void LcdNewchar(char ch, char font[]);

#endif