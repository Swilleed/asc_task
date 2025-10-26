#include "stm32f10x.h"
#include "OLED.h"
#include "stdlib.h"
#include "KEY.h"
#include "Delay.h"
#include "stdio.h"
#include "Encoder.h"

#define NULL 0
#define KEY_DOWN 0
#define KEY_UP 1
#define KEY_ENTER 2
#define KEY_BACK 3

typedef enum {
    LED_MEDIUM = 500,
    LED_SLOW = 1000,
    LED_FAST = 200
} LED_Speed;
typedef enum {
    DIR_CW = 0,
    DIR_CCW = 1
} LED_Direction;

// 菜单结构体定义
typedef struct menu {
    char *name;
    struct menu *parent;
    struct menu **child;
    uint8_t childcount;
    void (*func)(void);
    union {
        LED_Speed *led_speed_value;
        LED_Direction *led_direction_value;
        double *pid_value;
        void *generic_value;
    } value;
    uint8_t Index;
} Menu;

// 全局变量定义
int8_t MenuIndex = 0;
Menu *CurrentMenu;
LED_Speed led_speed_value = LED_MEDIUM;
LED_Direction led_direction_value = DIR_CW;
double pid_kp_value = 0.0;
double pid_ki_value = 0.0;
double pid_kd_value = 0.0;
// double Encoder_Value = 0.0;
static double current_encoder_delta = 0.0;
uint8_t KeyPressed;

// 辅助函数实现
static LED_Speed changeLEDSpeed(LED_Speed currentSpeed, uint8_t direction)
{
    if (direction == 1) {
        if (currentSpeed == LED_FAST) {
            return LED_MEDIUM;
        }
        else if (currentSpeed == LED_MEDIUM) {
            return LED_SLOW;
        }
        else {
            return LED_FAST;
        }
    }
    else {
        if (currentSpeed == LED_SLOW) {
            return LED_FAST;
        }
        else if (currentSpeed == LED_MEDIUM) {
            return LED_SLOW;
        }
        else {
            return LED_MEDIUM;
        }
    }
}
static LED_Direction changeLEDDirection(LED_Direction currentDirection)
{
    GPIO_SetBits(GPIOB, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
    return (currentDirection == DIR_CW) ? DIR_CCW : DIR_CW;
}

// 菜单功能实现
static void LED_Speed_Func(void)
{
    GPIO_SetBits(GPIOB, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
    if (Key_Check(KEY_UP, KEY_SINGLE)) {
        OLED_Clear();
        led_speed_value = changeLEDSpeed(led_speed_value, 1);
    }
    else if (Key_Check(KEY_DOWN, KEY_SINGLE)) {
        OLED_Clear();
        led_speed_value = changeLEDSpeed(led_speed_value, 0);
    }
    // else if (Key_Check(KEY_ENTER, KEY_SINGLE)) {
    //   MenuIndex = CurrentMenu->Index;
    // CurrentMenu = CurrentMenu->parent;
    //  }
}
static void LED_Direction_Func(void)
{
    if (Key_Check(KEY_DOWN, KEY_SINGLE)) {
        OLED_Clear();
        led_direction_value = changeLEDDirection(led_direction_value);
    }

    else if (Key_Check(KEY_UP, KEY_SINGLE)) {
        OLED_Clear();
        led_direction_value = changeLEDDirection(led_direction_value);
    }
    // else if (Key_Check(KEY_ENTER, KEY_SINGLE)) {
    //   MenuIndex = CurrentMenu->Index;
    // CurrentMenu = CurrentMenu->parent;
    //  }
}
static void PID_Kp_Func(void)
{
    if (Key_Check(KEY_UP, KEY_SINGLE) || Key_Check(KEY_UP, KEY_REPEAT)) {
        OLED_Clear();
        pid_kp_value += 0.1;
    }
    else if (Key_Check(KEY_DOWN, KEY_SINGLE) || Key_Check(KEY_DOWN, KEY_REPEAT)) {
        OLED_Clear();
        pid_kp_value -= 0.1;
    }
    //  else if (Key_Check(KEY_ENTER, KEY_SINGLE)) {
    //    MenuIndex = CurrentMenu->Index;
    //  CurrentMenu = CurrentMenu->parent;
    //}

    pid_kp_value -= current_encoder_delta;
}
static void PID_Ki_Func(void)
{

    if (Key_Check(KEY_UP, KEY_SINGLE) || Key_Check(KEY_UP, KEY_REPEAT)) {
        OLED_Clear();
        pid_ki_value += 0.1;
    }
    else if (Key_Check(KEY_DOWN, KEY_SINGLE) || Key_Check(KEY_DOWN, KEY_REPEAT)) {
        OLED_Clear();
        pid_ki_value -= 0.1;
    }
    // else if (Key_Check(KEY_ENTER, KEY_SINGLE)) {
    //   MenuIndex = CurrentMenu->Index;
    // CurrentMenu = CurrentMenu->parent;
    //    }
    pid_ki_value -= current_encoder_delta;
}
static void PID_Kd_Func(void)
{
    if (Key_Check(KEY_UP, KEY_SINGLE) || Key_Check(KEY_UP, KEY_REPEAT)) {
        OLED_Clear();
        pid_kd_value += 0.1;
    }
    else if (Key_Check(KEY_DOWN, KEY_SINGLE) || Key_Check(KEY_DOWN, KEY_REPEAT)) {
        OLED_Clear();
        pid_kd_value -= 0.1;
    }
    //  else if (Key_Check(KEY_ENTER, KEY_SINGLE)) {
    //    MenuIndex = CurrentMenu->Index;
    //  CurrentMenu = CurrentMenu->parent;
    //}
    pid_kd_value -= current_encoder_delta;
}

static char *DoubleToChar(double num)
{
    static char buffer[32];
    snprintf(buffer, sizeof(buffer), "%.1f", num);
    return buffer;
}

// 菜单定义
Menu Main, LED_Control, LED_speed, LED_direction, PID, kp, ki, kd, Image, Angle, Image_child, Angle_child;
Menu *Init_Menu(void)
{
    // 初始化菜单结构体
    Main.name = "Main Menu";
    Main.parent = NULL;
    Main.childcount = 4;
    Main.child = (Menu **)malloc(Main.childcount * sizeof(Menu *));
    Main.child[0] = &LED_Control;
    Main.child[1] = &PID;
    Main.child[2] = &Image;
    Main.child[3] = &Angle;
    Main.func = NULL;
    Main.Index = 0;
    LED_Control.name = "LED Control";
    LED_Control.parent = &Main;
    LED_Control.childcount = 2;
    LED_Control.child = (Menu **)malloc(LED_Control.childcount * sizeof(Menu *));
    LED_Control.child[0] = &LED_speed;
    LED_Control.child[1] = &LED_direction;
    LED_Control.func = NULL;
    LED_Control.Index = 0;
    LED_speed.name = "LED Speed";
    LED_speed.parent = &LED_Control;
    LED_speed.childcount = 0;
    LED_speed.child = NULL;
    LED_speed.func = LED_Speed_Func;
    LED_speed.value.led_speed_value = &led_speed_value;
    LED_speed.Index = 0;
    LED_direction.name = "LED Dir";
    LED_direction.parent = &LED_Control;
    LED_direction.childcount = 0;
    LED_direction.child = NULL;
    LED_direction.func = LED_Direction_Func;
    LED_direction.value.led_direction_value = &led_direction_value;
    LED_direction.Index = 1;
    PID.name = "PID";
    PID.parent = &Main;
    PID.childcount = 3;
    PID.child = (Menu **)malloc(PID.childcount * sizeof(Menu *));
    PID.child[0] = &kp;
    PID.child[1] = &ki;
    PID.child[2] = &kd;
    PID.func = NULL;
    PID.Index = 1;
    kp.name = "Kp";
    kp.parent = &PID;
    kp.childcount = 0;
    kp.child = NULL;
    kp.func = PID_Kp_Func;
    kp.value.pid_value = &pid_kp_value;
    kp.Index = 0;
    ki.name = "Ki";
    ki.parent = &PID;
    ki.childcount = 0;
    ki.child = NULL;
    ki.func = PID_Ki_Func;
    ki.value.pid_value = &pid_ki_value;
    ki.Index = 1;
    kd.name = "Kd";
    kd.parent = &PID;
    kd.childcount = 0;
    kd.child = NULL;
    kd.func = PID_Kd_Func;
    kd.value.pid_value = &pid_kd_value;
    kd.Index = 2;
    Image.name = "Image";
    Image.parent = &Main;
    Image.childcount = 1;
    Image.child = (Menu **)malloc(Image.childcount * sizeof(Menu *));
    Image.child[0] = &Image_child;
    Image.func = NULL;
    Image.value.generic_value = NULL;
    Image.Index = 2;
    Image_child.name = "Image";
    Image_child.parent = &Image;
    Image_child.childcount = 0;
    Image.child = (Menu **)malloc(Image.childcount * sizeof(Menu *));
    Image.child[0] = &Image_child;
    Image.func = NULL;
    Image.value.generic_value = NULL;
    Image.Index = 2;
    Angle.name = "Angle";
    Angle.parent = &Main;
    Angle.childcount = 1;
    Angle.child = (Menu **)malloc(Angle.childcount * sizeof(Menu *));
    Angle.child[0] = &Angle_child;
    Angle.func = NULL;
    Angle.value.generic_value = NULL;
    Angle.Index = 3;
    Angle_child.name = "Angle";
    Angle_child.parent = &Angle;
    Angle_child.childcount = 0;
    Angle_child.child = NULL;
    Angle_child.func = NULL;
    Angle_child.value.generic_value = NULL;
    Angle_child.Index = 0;
    CurrentMenu = &Main;

    return &Main;
}

void displayMenu(void)
{
    // OLED_Clear();

    // 按键处理部分
    current_encoder_delta = Encoder_Get();

    //  OLED_ShowString(3, 10, "Enc:");
    // OLED_ShowSignedNum(4, 10, (int16_t)(current_encoder_delta * 10), 4);
    if (CurrentMenu->func == NULL) {
        if ((Key_Check(KEY_UP, KEY_SINGLE) || Key_Check(KEY_UP, KEY_REPEAT)) && CurrentMenu->childcount > 0) {
            MenuIndex = (MenuIndex == CurrentMenu->childcount - 1) ? 0 : MenuIndex + 1;
            OLED_Clear();
        }
        if ((Key_Check(KEY_DOWN, KEY_SINGLE) || Key_Check(KEY_DOWN, KEY_SINGLE)) && CurrentMenu->childcount > 0) {
            MenuIndex = (MenuIndex == 0) ? CurrentMenu->childcount - 1 : MenuIndex - 1;
            OLED_Clear();
        }
    }
    if (Key_Check(KEY_ENTER, KEY_SINGLE)) {
        OLED_Clear();
        if (CurrentMenu->childcount == 0 && CurrentMenu->func != NULL) {
            // CurrentMenu->func();

            MenuIndex = CurrentMenu->Index;
            CurrentMenu = CurrentMenu->parent;
        }
        else {
            CurrentMenu = CurrentMenu->child[MenuIndex];
            if (CurrentMenu->child[MenuIndex]->func == NULL) {
                MenuIndex = 0;
            }
        }
    }
    if (Key_Check(KEY_BACK, KEY_SINGLE) && CurrentMenu->parent != NULL) {
        OLED_Clear();
        MenuIndex = CurrentMenu->Index;
        CurrentMenu = CurrentMenu->parent;
    }

    // 当前菜单
    if (CurrentMenu->func != NULL) {
        OLED_ShowString(1, 1, CurrentMenu->parent->name);
    }
    else {
        OLED_ShowString(1, 1, CurrentMenu->name);
    }
    char *speedStr;
    switch (led_speed_value) {
    case LED_SLOW:
        speedStr = "1";
        break;
    case LED_MEDIUM:
        speedStr = "0";
        break;
    case LED_FAST:
        speedStr = "2";
        break;
    default:
        speedStr = "UNK";
        break;
    }
    char *dirStr = (led_direction_value == DIR_CW) ? "0" : "1";
    // 菜单显示部分
    for (uint8_t i = 0; i < CurrentMenu->childcount; i++) {
        OLED_ShowString(i + 2, 1, (i == MenuIndex) ? ">" : " ");
        OLED_ShowString(i + 2, 2, CurrentMenu->child[i]->name);

        if (CurrentMenu == Main.child[1]) {
            for (uint8_t i = 0; i < PID.childcount; i++) {
                OLED_ShowString(i + 2, 13, DoubleToChar(*(PID.child[i]->value.pid_value)));
            }
        }
        if (CurrentMenu == Main.child[0]) {
            OLED_ShowString(2, 14, speedStr);
            OLED_ShowString(3, 14, dirStr);
        }
    }

    // 功能指示部分
    if (CurrentMenu->func != NULL) {
        OLED_ShowString(1, 14, "E");
    }

    // 全局变量显示部分

    // 根据当前菜单调用相应的功能显示
    if (CurrentMenu->func == LED_Speed_Func) {
        LED_Speed_Func();

        // 一个菜单中所有子菜单数据都显示
        OLED_ShowString(2, 14, speedStr);
        OLED_ShowString(3, 14, dirStr);

        for (uint8_t i = 0; i < CurrentMenu->parent->childcount; i++) {
            OLED_ShowString(i + 2, 1, (i == MenuIndex) ? ">" : " ");
            OLED_ShowString(i + 2, 3, CurrentMenu->parent->child[i]->name);
        }
    }
    else if (CurrentMenu->func == LED_Direction_Func) {
        LED_Direction_Func();

        OLED_ShowString(2, 14, speedStr);
        OLED_ShowString(3, 14, dirStr);
        for (uint8_t i = 0; i < CurrentMenu->parent->childcount; i++) {
            OLED_ShowString(i + 2, 1, (i == MenuIndex) ? ">" : " ");
            OLED_ShowString(i + 2, 3, CurrentMenu->parent->child[i]->name);
        }
    }
    else if (CurrentMenu->func == PID_Kp_Func) {
        PID_Kp_Func();
        // OLED_ShowSignedNum(1, 5, (uint16_t)Encoder_Get() * 10, 2);

        OLED_ShowString(2, 13, DoubleToChar(pid_kp_value));
        OLED_ShowString(3, 13, DoubleToChar(pid_ki_value));
        OLED_ShowString(4, 13, DoubleToChar(pid_kd_value));
        for (uint8_t i = 0; i < CurrentMenu->parent->childcount; i++) {
            OLED_ShowString(i + 2, 1, (i == MenuIndex) ? ">" : " ");
            OLED_ShowString(i + 2, 3, CurrentMenu->parent->child[i]->name);
        }
    }
    else if (CurrentMenu->func == PID_Ki_Func) {
        PID_Ki_Func();
        OLED_ShowString(2, 13, DoubleToChar(pid_kp_value));
        OLED_ShowString(3, 13, DoubleToChar(pid_ki_value));
        OLED_ShowString(4, 13, DoubleToChar(pid_kd_value));
        for (uint8_t i = 0; i < CurrentMenu->parent->childcount; i++) {
            OLED_ShowString(i + 2, 1, (i == MenuIndex) ? ">" : " ");
            OLED_ShowString(i + 2, 3, CurrentMenu->parent->child[i]->name);
        }
    }
    else if (CurrentMenu->func == PID_Kd_Func) {
        PID_Kd_Func();
        OLED_ShowString(2, 13, DoubleToChar(pid_kp_value));
        OLED_ShowString(3, 13, DoubleToChar(pid_ki_value));
        OLED_ShowString(4, 13, DoubleToChar(pid_kd_value));
        for (uint8_t i = 0; i < CurrentMenu->parent->childcount; i++) {
            OLED_ShowString(i + 2, 1, (i == MenuIndex) ? ">" : " ");
            OLED_ShowString(i + 2, 3, CurrentMenu->parent->child[i]->name);
        }
    }

    /*if (CurrentMenu->func != NULL) {
        if (Key_Check(KEY_ENTER, KEY_SINGLE)) {
            MenuIndex = CurrentMenu->Index;
            CurrentMenu = CurrentMenu->parent;
        }
    }*/

    /*
    if (Key_Check(KEY_UP, KEY_SINGLE)) {
        MenuIndex = (MenuIndex == 0) ? CurrentMenu->childcount - 1 : MenuIndex - 1;
    }
    if (Key_Check(KEY_DOWN, KEY_SINGLE)) {
        MenuIndex = (MenuIndex == CurrentMenu->childcount - 1) ? 0 : MenuIndex + 1;
    }
    if (Key_Check(KEY_ENTER, KEY_SINGLE)) {
        if (CurrentMenu->childcount == 0 && CurrentMenu->func != NULL) {
            CurrentMenu->func();
        }
        else {
            CurrentMenu = CurrentMenu->child[MenuIndex];
            MenuIndex = 0;
        }
    }
    if (Key_Check(KEY_BACK,KEY_SINGLE)) {
        CurrentMenu = CurrentMenu->parent;
        MenuIndex = 0;
    }
    for (uint8_t i = 0; i < CurrentMenu->childcount; i++) {
        if (i == MenuIndex) {
            OLED_ShowString(i + 2, 1, ">");
        }
        else {
            OLED_ShowString(i + 2, 1, " ");
        }
    }*/

    // Delay_ms(2000);
}

void displayMainMenu(void)
{

    // OLED_Clear();
    current_encoder_delta = Encoder_Get();

    // OLED_ShowString(3, 10, "Enc:");
    // OLED_ShowSignedNum(4, 10, (int16_t)(current_encoder_delta * 10), 4);

    for (uint8_t i = 0; i < Main.childcount; i++) {
        OLED_ShowString(i + 1, 2, Main.child[i]->name);
    }
    for (uint8_t i = 0; i < Main.childcount; i++) {
        if (i == MenuIndex) {
            OLED_ShowString(i + 1, 1, ">");
        }
        else {
            OLED_ShowString(i + 1, 1, " ");
        }
    }

    if ((Key_Check(KEY_UP, KEY_SINGLE) || Key_Check(KEY_UP, KEY_REPEAT)) && CurrentMenu->childcount > 0) {
        OLED_Clear();
        MenuIndex = (MenuIndex == CurrentMenu->childcount - 1) ? 0 : MenuIndex + 1;
    }
    if ((Key_Check(KEY_DOWN, KEY_SINGLE) || Key_Check(KEY_DOWN, KEY_REPEAT)) && CurrentMenu->childcount > 0) {
        OLED_Clear();
        MenuIndex = (MenuIndex == 0) ? Main.childcount - 1 : MenuIndex - 1;
    }
    if (Key_Check(KEY_ENTER, KEY_SINGLE)) {
        OLED_Clear();
        CurrentMenu = CurrentMenu->child[MenuIndex];

        MenuIndex = 0;
    }
    if (Key_Check(KEY_BACK, KEY_SINGLE) && CurrentMenu->parent != NULL) {
        OLED_Clear();
        CurrentMenu = CurrentMenu->parent;
        MenuIndex = 0;
    }

    // Delay_ms(2000);
}
/*void handleMenuInput(void)
{
    if (Key_Check(KEY_UP, KEY_SINGLE) && CurrentMenu->childcount > 0) {
        MenuIndex = (MenuIndex == CurrentMenu->childcount - 1) ? 0 : MenuIndex + 1;
    }
    if (Key_Check(KEY_DOWN, KEY_SINGLE)) {
        MenuIndex = (MenuIndex == 0) ? Main.childcount - 1 : MenuIndex - 1;
    }
    if (Key_Check(KEY_ENTER, KEY_SINGLE)) {
        CurrentMenu = CurrentMenu->child[MenuIndex];
        MenuIndex = 0;
    }
    if (Key_Check(KEY_BACK, KEY_SINGLE) && CurrentMenu->parent != NULL) {
        CurrentMenu = CurrentMenu->parent;
        MenuIndex = 0;
    }
}*/
