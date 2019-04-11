#ifndef __KEYPAD_H__
#define __KEYPAD_H__

#define SW0 0x18
#define SW1 0x14
#define SW2 0x12
#define SW3 0x11
#define SW4 0x28
#define SW5 0x24
#define SW6 0x22
#define SW7 0x21
#define SW8 0x48
#define SW9 0x44
#define SW10 0x42
#define SW11 0x41
#define SW12 0x88
#define SW13 0x84
#define SW14 0x82
#define SW15 0x81
#define NO_KEY 0x00

void KeyInit(void);
unsigned char KeyInput(void);

#endif