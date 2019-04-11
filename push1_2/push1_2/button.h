#ifndef __BUTTON_H__
#define __BUTTON_H__

#define BTN_SW0 0x01
#define BTN_SW1 0x02
#define BTN_SW2 0x04
#define BTN_SW01 (BTN_SW0 | BTN_SW1)
#define BTN_SW02 (BTN_SW0 | BTN_SW2)
#define BTN_SW12 (BTN_SW1 | BTN_SW2)
#define BTN_SW012 (BTN_SW0 | BTN_SW1 | BTN_SW2)
#define NO_BTN 0x00

#define BTN_MASK (BTN_SW0 | BTN_SW1 | BTN_SW2)

void BtnInit(void);
unsigned char BtnInput(void);
unsigned char BtnInput_Press(unsigned char *pressed);

#endif