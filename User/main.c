#include "stm32f10x.h"
#include "Delay.h"
#include "KEY.h"
#include "OLED.h"
#include "Encoder.h"
#include "Menu.h"
#include <string.h>
#include "Timer.h"
#include "LED.h"

// extern float Encoder_Count;

int main()
{
    Timer_Init();
    OLED_Init();
    Encoder_Init();
    OLED_ShowString(1, 1, "Starting...");
    Delay_ms(1000);
    OLED_Clear();
    Key_Init();
    LED_Init();
    Menu *Main = Init_Menu();
    CurrentMenu = Main;

    while (1) {

        // pid_kp_value += Encoder_Get();
        // OLED_ShowSignedNum(1, 5, Num, 5);
        if (CurrentMenu == Main) {
            displayMainMenu();
        }
        else {
            displayMenu();
        }
        // OLED_ShowSignedNum(4, 10, pid_kd_value, 5);
        //  displayMenu();
    }
}

void TIM2_IRQHandler(void)
{
    Key_Tick();
    LED_Tick();
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET) {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}
/*#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"

// 简单的GPIO测试
void GPIO_Test(void)
{
    // 初始化PB0和PB1为输入
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // 上拉输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    OLED_Init();
    OLED_ShowString(1, 1, "GPIO Test:");
    OLED_ShowString(2, 1, "PB0:");
    OLED_ShowString(3, 1, "PB1:");

    while (1) {
        // 显示引脚电平状态
        uint8_t pb0_state = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0);
        uint8_t pb1_state = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1);

        OLED_ShowString(2, 6, pb0_state ? "HIGH" : "LOW ");
        OLED_ShowString(3, 6, pb1_state ? "HIGH" : "LOW ");

        // 显示编码器状态组合
        if (pb0_state && pb1_state) {
            OLED_ShowString(4, 1, "State: 11");
        }
        else if (pb0_state && !pb1_state) {
            OLED_ShowString(4, 1, "State: 10");
        }
        else if (!pb0_state && pb1_state) {
            OLED_ShowString(4, 1, "State: 01");
        }
        else {
            OLED_ShowString(4, 1, "State: 00");
        }

        Delay_ms(100);
    }
}

int main(void)
{
    GPIO_Test();
}*/

/*
#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"

volatile int32_t encoder_count = 0;
volatile uint32_t exti0_count = 0;
volatile uint32_t exti1_count = 0;

void Encoder_Simple_Init(void)
{
    // 1. 开启时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

    // 2. 配置GPIO为上拉输入
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // 3. 配置外部中断
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1);

    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line = EXTI_Line0 | EXTI_Line1;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    // 4. 配置NVIC
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
    NVIC_Init(&NVIC_InitStructure);
}

void EXTI0_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line0) == SET) {
        exti0_count++;

        // 简单判断方向
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0) {
            encoder_count--;
        }
        else {
            encoder_count++;
        }

        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}

void EXTI1_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line1) == SET) {
        exti1_count++;

        // 简单判断方向
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 0) {
            encoder_count++;
        }
        else {
            encoder_count--;
        }

        EXTI_ClearITPendingBit(EXTI_Line1);
    }
}

int main(void)
{
    Encoder_Simple_Init();
    OLED_Init();

    OLED_ShowString(1, 1, "Simple Encoder Test");
    OLED_ShowString(2, 1, "EXTI0: 0");
    OLED_ShowString(3, 1, "EXTI1: 0");
    OLED_ShowString(4, 1, "Count: 0");

    while (1) {
        OLED_ShowNum(2, 7, exti0_count, 5);
        OLED_ShowNum(3, 7, exti1_count, 5);
        OLED_ShowSignedNum(4, 8, encoder_count, 5);
        Delay_ms(50);
    }
}*/

// 调试信息显示
/*void ShowDebugInfo(void)
{
    static uint32_t last_show = 0;
    static uint32_t count = 0;

    count++;
    if (count - last_show > 20) { // 每20次循环显示一次
        OLED_ShowString(4, 1, "OK");
        last_show = count;
    }
}

int main()
{
    Timer_Init();
    OLED_Init();
    Encoder_Init();

    OLED_ShowString(1, 1, "Integrated Test");
    Delay_ms(1000);
    OLED_Clear();

    Key_Init();
    LED_Init();
    Menu *Main = Init_Menu();
    CurrentMenu = Main;

    while (1) {
        ShowDebugInfo(); // 显示系统运行状态

        if (CurrentMenu == Main) {
            displayMainMenu();
        }
        else {
            displayMenu();
        }

        Delay_ms(50); // 控制刷新率
    }
}

void TIM2_IRQHandler(void)
{
    Key_Tick();
    LED_Tick();
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET) {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}*/

// main.c - 最简化测试
/*#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"
#include "Encoder.h"

extern volatile uint32_t exti0_count;
extern volatile uint32_t exti1_count;
volatile double encoder_total;

int main(void)
{
    // 只初始化最基础的功能
    OLED_Init();
    Encoder_Init();
    Timer_Init();
    LED_Init();

    OLED_ShowString(1, 1, "Minimal Test");
    OLED_ShowString(2, 1, "EXTI0: 0");
    OLED_ShowString(3, 1, "EXTI1: 0");
    OLED_ShowString(4, 1, "Total: 0.0");

    while (1) {
        // 获取编码器值但不清零
        double current_val = Encoder_Get();
        if (current_val != 0) {
            encoder_total += current_val;
        }

        // 显示中断计数和总值
        OLED_ShowNum(2, 7, exti0_count, 5);
        OLED_ShowNum(3, 7, exti1_count, 5);
        OLED_ShowSignedNum(4, 7, (int16_t)(encoder_total * 10), 5);

        Delay_ms(50);
    }
}

extern double Encoder_Count;
// 在 Encoder.c 中添加中断计数
void EXTI0_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line0) == SET) {
        exti0_count++; // 中断计数

        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 0) {
            if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 1) {
                Encoder_Count -= 0.1;
            }
            else {
                Encoder_Count += 0.1;
            }
        }
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}

void EXTI1_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line1) == SET) {
        exti1_count++; // 中断计数

        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0) {
            if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 1) {
                Encoder_Count += 0.1;
            }
            else {
                Encoder_Count -= 0.1;
            }
        }
        EXTI_ClearITPendingBit(EXTI_Line1);
    }
}

void TIM2_IRQHandler(void)
{
    Key_Tick();
    LED_Tick();
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET) {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}
    */