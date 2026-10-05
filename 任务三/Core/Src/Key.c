#include "Key.h"
#include "tim.h"

struct Key_Info
{
    Key_InitStruct KeyPin;
    uint8_t LastState;
    uint8_t NowState;
    uint8_t KeyState;
    uint32_t KeyTick;
} KeyInfo[KEYNUM];

Key_Class Key = {0};

void Key_InitKey(uint8_t KeyNum, Key_InitStruct KeyPinList[])
{
    if (KeyNum >= KEYNUM)
    {
        return;
    }
    Key.KeyNum = KeyNum;
    for (uint8_t i = 0; i < KeyNum; i++)
    {
        KeyInfo[i].KeyPin = KeyPinList[i];
        KeyInfo[i].LastState = UNPRESSED;
        KeyInfo[i].NowState  = UNPRESSED;
        KeyInfo[i].KeyState  = UNPRESSED;
        KeyInfo[i].KeyTick = 0;
    }
}

void Key_AddKey(Key_InitStruct KeyPin)
{
    if (Key.KeyNum >= KEYNUM)
    {
        return;
    }
    KeyInfo[Key.KeyNum].KeyPin = KeyPin;
    KeyInfo[Key.KeyNum].LastState = UNPRESSED;
    KeyInfo[Key.KeyNum].NowState  = UNPRESSED;
    KeyInfo[Key.KeyNum].KeyState  = UNPRESSED;
    KeyInfo[Key.KeyNum].KeyTick = 0;
    Key.KeyNum++;
}

uint8_t Key_GetState(uint8_t KeyID)
{
    uint8_t PinState;
    PinState = HAL_GPIO_ReadPin(KeyInfo[KeyID].KeyPin.GPIOx, KeyInfo[KeyID].KeyPin.GPIO_Pin);
    if (KeyInfo[KeyID].KeyPin.Pull == GPIO_PULLDOWN)
    {
        if (PinState == GPIO_PIN_SET)
        {
            return PRESSED;
        }
        else
        {
            return UNPRESSED;
        }
    }
    if (KeyInfo[KeyID].KeyPin.Pull == GPIO_PULLUP)
    {
        if (PinState == GPIO_PIN_SET)
        {
            return UNPRESSED;
        }
        else
        {
            return PRESSED;
        }
    }
}

void Key_Loop(void)
{
    for(uint8_t i = 0; i < Key.KeyNum; i++)
    {
        KeyInfo[i].LastState = KeyInfo[i].NowState;
        KeyInfo[i].NowState = Key_GetState(i);
        switch (KeyInfo[i].KeyState)
        {
        case UNPRESSED:
			if (KeyInfo[i].LastState == UNPRESSED && KeyInfo[i].NowState == PRESSED)
            {
                KeyInfo[i].KeyState = PRESSED;
                KeyInfo[i].KeyTick = HAL_GetTick();
            }
			break;
		case PRESSED:
			if ((HAL_GetTick() - KeyInfo[i].KeyTick) >= LONG_PRESS_TIME)
            {
                KeyInfo[i].KeyState = LONG_PRESS;
                KeyInfo[i].KeyTick = HAL_GetTick();
            }
            if (KeyInfo[i].LastState == PRESSED && KeyInfo[i].NowState == UNPRESSED)
            {
                if ((HAL_GetTick() - KeyInfo[i].KeyTick) >= 40)
                {
                    KeyInfo[i].KeyState = WAIT_PRESS;
                    KeyInfo[i].KeyTick = HAL_GetTick();
                }
                else
                {
                    KeyInfo[i].KeyState = UNPRESSED;
                    KeyInfo[i].KeyTick = 0;
                }
            }
			break;
		case WAIT_PRESS:
			if ((HAL_GetTick() - KeyInfo[i].KeyTick) >= WAIT_PRESS_MAX_GAP)
            {
                KeyInfo[i].KeyState = UNPRESSED;
                Key.KeyEvent[i] = SINGLE;
                KeyInfo[i].KeyTick = 0;
            }
            if (KeyInfo[i].LastState == UNPRESSED && KeyInfo[i].NowState == PRESSED && (HAL_GetTick() - KeyInfo[i].KeyTick) > WAIT_PRESS_MIN_GAP)
            {
                
                KeyInfo[i].KeyState = WAIT_END;
                KeyInfo[i].KeyTick = 0;
            }
			break;
		case WAIT_END:
			if (KeyInfo[i].LastState == PRESSED && KeyInfo[i].NowState == UNPRESSED)
            {
                KeyInfo[i].KeyState = UNPRESSED;
                Key.KeyEvent[i] = DOUBLE;
            }
			break;
		case LONG_PRESS:
			if ((HAL_GetTick() - KeyInfo[i].KeyTick) >= REPEAT_GAP)
            {
                Key.KeyEvent[i] = REPEAT;
                KeyInfo[i].KeyTick = HAL_GetTick();
            }
            if (KeyInfo[i].LastState == PRESSED && KeyInfo[i].NowState == UNPRESSED)
            {
                KeyInfo[i].KeyState = UNPRESSED;
                Key.KeyEvent[i] = LONG_PRESS;
                KeyInfo[i].KeyTick = 0;
            }
			break;
        }
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2)
    {
        Key.Loop = 1;
    }
}