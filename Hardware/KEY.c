#include "stm32f10x.h" // Device header

#define KEY_PRESSED 1
#define KEY_UNPRESSED 0
// 位掩码
#define KEY_HOLD 0x01
#define KEY_DOWN_A 0x02
#define KEY_UP_A 0x04
#define KEY_SINGLE 0x08
#define KEY_DOUBLE 0x10
#define KEY_LONG 0x20
#define KEY_REPEAT 0x40
// 时间设定
#define KEY_TIME_DOUBLE 0
#define KEY_TIME_LONG 1000
#define KEY_TIME_REPEAT 100
// 按键数量
#define KEY_COUNT 4

#define KEY_1 0
#define KEY_2 1
#define KEY_3 2
#define KEY_4 3

uint8_t Key_Flag[KEY_COUNT];

void Key_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_2 | GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/*uint8_t Key_Check(uint8_t n, uint8_t Flag)
{
    if (Key_Flag[n] & Flag) {
        if (Key_Flag[n] != KEY_HOLD) {
            Key_Flag[n] &= ~Flag;
        }
        return 1;
    }
    return 0;
}*/

uint8_t Key_GetState(uint8_t n)
{
    if (n == 0) {
        if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4) == 0) {
            return KEY_PRESSED;
        }
    }
    if (n == 1) {
        if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2) == 0) {
            return KEY_PRESSED;
        }
    }
    if (n == 2) {
        if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 0) {
            return KEY_PRESSED;
        }
    }
    if (n == 3) {
        if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_14) == 0) {
            return KEY_PRESSED;
        }
    }

    return KEY_UNPRESSED;
}

uint8_t Key_Check(uint8_t n, uint8_t Flag)
{
    if (Key_Flag[n] & Flag) {
        if (Flag != KEY_HOLD) {
            Key_Flag[n] &= ~Flag;
        }
        return 1;
    }
    return 0;
}

void Key_Tick(void)
{
    static uint8_t Count, i;
    static uint8_t CurrState[KEY_COUNT], PrevState[KEY_COUNT];
    static uint8_t S[KEY_COUNT];
    static uint16_t Time[KEY_COUNT];

    for (i = 0; i < KEY_COUNT; i++) {
        if (Time[i] > 0) {
            Time[i]--;
        }
    }

    Count++;
    if (Count >= 20) {
        Count = 0;

        for (i = 0; i < KEY_COUNT; i++) {
            PrevState[i] = CurrState[i];
            CurrState[i] = Key_GetState(i);

            if (CurrState[i] == KEY_PRESSED) {
                Key_Flag[i] |= KEY_HOLD;
            }
            else {
                Key_Flag[i] &= ~KEY_HOLD;
            }

            if (CurrState[i] == KEY_PRESSED && PrevState[i] == KEY_UNPRESSED) {
                Key_Flag[i] |= KEY_DOWN_A;
            }

            if (CurrState[i] == KEY_UNPRESSED && PrevState[i] == KEY_PRESSED) {
                Key_Flag[i] |= KEY_UP_A;
            }

            if (S[i] == 0) {
                if (CurrState[i] == KEY_PRESSED) {
                    Time[i] = KEY_TIME_LONG;
                    S[i] = 1;
                }
            }
            else if (S[i] == 1) {
                if (CurrState[i] == KEY_UNPRESSED) {
                    Time[i] = KEY_TIME_DOUBLE;
                    S[i] = 2;
                }
                else if (Time[i] == 0) {
                    Time[i] = KEY_TIME_REPEAT;
                    Key_Flag[i] |= KEY_LONG;
                    S[i] = 4;
                }
            }
            else if (S[i] == 2) {
                if (CurrState[i] == KEY_PRESSED) {
                    Key_Flag[i] |= KEY_DOUBLE;
                    S[i] = 3;
                }
                else if (Time[i] == 0) {
                    Key_Flag[i] |= KEY_SINGLE;
                    S[i] = 0;
                }
            }
            else if (S[i] == 3) {
                if (CurrState[i] == KEY_UNPRESSED) {
                    S[i] = 0;
                }
            }
            else if (S[i] == 4) {
                if (CurrState[i] == KEY_UNPRESSED) {
                    S[i] = 0;
                }
                else if (Time[i] == 0) {
                    Time[i] = KEY_TIME_REPEAT;
                    Key_Flag[i] |= KEY_REPEAT;
                    S[i] = 4;
                }
            }
        }
    }
}
