#ifndef __KEY_H__
#define __KEY_H__

#include "stm32f1xx_hal.h"

#define KEYNUM          16

#define WAIT_PRESS_MIN_GAP	80
#define WAIT_PRESS_MAX_GAP	200
#define LONG_PRESS_TIME	    1000
#define REPEAT_GAP 		    100

#define UNPRESSED	0
#define PRESSED		1
#define WAIT_PRESS	2
#define WAIT_END    3
#define LONG_PRESS	4
#define SINGLE		5
#define DOUBLE		6
#define REPEAT		7

typedef struct
{
    GPIO_TypeDef *GPIOx;
    uint16_t GPIO_Pin;
    uint8_t Pull;
} Key_InitStruct;

typedef struct 
{
    uint8_t KeyNum;
    uint8_t Loop;
    uint8_t KeyEvent[KEYNUM];
} Key_Class;

extern Key_Class Key;

void Key_InitKey(uint8_t KeyNum, Key_InitStruct KeyPinList[]);
void Key_AddKey(Key_InitStruct KeyPin);
void Key_Loop(void);

#endif