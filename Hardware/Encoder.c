/*
#include "stm32f10x.h" // Device header

volatile double Encoder_Count; // 全局变量，用于计数旋转编码器的增量值

void Encoder_Init(void)
{

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // 开启GPIOB的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);  // 开启AFIO的时钟，外部中断必须开启AFIO的时钟

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure); // 将PB0和PB1引脚初始化为上拉输入

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0); // 将外部中断的0号线映射到GPIOB，即选择PB0为外部中断引脚
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1); // 将外部中断的1号线映射到GPIOB，即选择PB1为外部中断引脚

    EXTI_InitTypeDef EXTI_InitStructure;                    // 定义结构体变量
    EXTI_InitStructure.EXTI_Line = EXTI_Line0 | EXTI_Line1; // 选择配置外部中断的0号线和1号线
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;               // 指定外部中断线使能
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;     // 指定外部中断线为中断模式
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; // 指定外部中断线为下降沿触发
    EXTI_Init(&EXTI_InitStructure);                         // 将结构体变量交给EXTI_Init，配置EXTI外设

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 配置NVIC为分组2
                                                    // 即抢占优先级范围：0~3，响应优先级范围：0~3
                                                    // 此分组配置在整个工程中仅需调用一次
                                                    // 若有多个中断，可以把此代码放在main函数内，while循环之前
                                                    // 若调用多次配置分组的代码，则后执行的配置会覆盖先执行的配置

    NVIC_InitTypeDef NVIC_InitStructure;                      // 定义结构体变量
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;          // 选择配置NVIC的EXTI0线
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // 指定NVIC线路使能
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // 指定NVIC线路的抢占优先级为1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;        // 指定NVIC线路的响应优先级为1
    NVIC_Init(&NVIC_InitStructure);                           // 将结构体变量交给NVIC_Init，配置NVIC外设

    NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;          // 选择配置NVIC的EXTI1线
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // 指定NVIC线路使能
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // 指定NVIC线路的抢占优先级为1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;        // 指定NVIC线路的响应优先级为2
    NVIC_Init(&NVIC_InitStructure);                           // 将结构体变量交给NVIC_Init，配置NVIC外设
}

double Encoder_Get(void)
{

    double Temp;
    Temp = Encoder_Count;
    Encoder_Count = 0;
    return Temp;
}

void EXTI0_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line0) == SET) {

        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 0) // 确认PB0下降沿
        {
            // PB0触发时，通过PB1电平判断方向
            if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 1)
                Encoder_Count += 0.1; // 正转
            else
                Encoder_Count -= 0.1; // 反转
        }
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}

void EXTI0_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line0) == SET) // 判断是否是外部中断0号线触发的中断
    {

        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 0) {
            if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 1) // PB0的下降沿触发中断，此时检测另一相PB1的电平，目的是判断旋转方向
            {
                Encoder_Count -= 0.1; // 此方向定义为反转，计数变量自减
            }
        }
        EXTI_ClearITPendingBit(EXTI_Line0); // 清除外部中断0号线的中断标志位
        // 中断标志位必须清除
        // 否则中断将连续不断地触发，导致主程序卡死
    }
}

void EXTI1_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line1) == SET) // 判断是否是外部中断1号线触发的中断
    {

        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0) {
            if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 1) // PB1的下降沿触发中断，此时检测另一相PB0的电平，目的是判断旋转方向
            {
                Encoder_Count += 0.1; // 此方向定义为正转，计数变量自增
            }
        }
        EXTI_ClearITPendingBit(EXTI_Line1); // 清除外部中断1号线的中断标志位
                                            // 中断标志位必须清除
                                            // 否则中断将连续不断地触发，导致主程序卡死
    }
}
*/

// Encoder.c - 使用已验证可工作的中断代码
#include "stm32f10x.h"

double Encoder_Count = 0;
volatile uint32_t exti0_count = 0;
volatile uint32_t exti1_count = 0;

void Encoder_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1);

    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line = EXTI_Line0 | EXTI_Line1;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // 高优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // 高优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStructure);
}

double Encoder_Get(void)
{
    double Temp;
    Temp = Encoder_Count;
    Encoder_Count = 0;
    return Temp;
}

/*void EXTI0_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line0) == SET) {
        exti0_count++;

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
        exti1_count++;

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
}*/

void EXTI0_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line0) == SET) {
        /*如果出现数据乱跳的现象，可再次判断引脚电平，以避免抖动*/
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 0) {
            if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0) {
                Encoder_Count--;
            }
        }
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}

void EXTI1_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line1) == SET) {
        /*如果出现数据乱跳的现象，可再次判断引脚电平，以避免抖动*/
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0) {
            if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 0) {
                Encoder_Count++;
            }
        }
        EXTI_ClearITPendingBit(EXTI_Line1);
    }
}
