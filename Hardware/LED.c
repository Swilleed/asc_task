#include "stm32f10x.h" // Device header
#include "Menu.h"
uint16_t LED1_Count = 0;

void LED_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_SetBits(GPIOB, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
}

void LED1_ON(void)
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_12);
}
void LED1_OFF(void)
{
    GPIO_SetBits(GPIOB, GPIO_Pin_12);
}

void LED2_ON(void)
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_13);
}
void LED2_OFF(void)
{
    GPIO_SetBits(GPIOB, GPIO_Pin_13);
}

void LED3_ON(void)
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_14);
}
void LED3_OFF(void)
{
    GPIO_SetBits(GPIOB, GPIO_Pin_14);
}

void LED4_ON(void)
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_15);
}
void LED4_OFF(void)
{
    GPIO_SetBits(GPIOB, GPIO_Pin_15);
}

void LED_Tick(void)
{
    LED1_Count++;
    LED1_Count %= led_speed_value * 4;

    if (led_direction_value == DIR_CW) {
        switch (LED1_Count / led_speed_value) {
        case 0:
            LED1_ON();
            break;
        // case 1:
        //   LED1_OFF();
        // break;
        case 1:
            LED2_ON();
            break;
        // case 3:
        //   LED2_OFF();
        // break;
        case 2:
            LED3_ON();
            break;
        // case 5:
        //   LED3_OFF();
        // break;
        case 3:
            LED4_ON();
            break;
            // case 7:
            //   LED4_OFF();
            //            break;

        default:
            break;
        }
    }
    else {
        switch (LED1_Count / led_speed_value) {
        case 0:
            LED4_ON();
            break;
            // case 1:
            //   LED4_OFF();
            // break;
        case 1:
            LED3_ON();
            break;
        // case 3:
        //   LED3_OFF();
        // break;
        case 2:
            LED2_ON();
            break;
        // case 5:
        //   LED2_OFF();
        // break;
        case 3:
            LED1_ON();
            break;
            // case 7:
            //   LED1_OFF();
            // break;

        default:
            break;
        }
    }

    if (LED1_Count % led_speed_value == 0) {
        GPIO_SetBits(GPIOB, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
    }
}
