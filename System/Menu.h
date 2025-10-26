#ifndef __MENU_H
#define __MENU_H

#include <stdint.h>

#define NULL 0

typedef enum {
    LED_MEDIUM = 500,
    LED_SLOW = 1000,
    LED_FAST = 200
} LED_Speed;
typedef enum {
    DIR_CW = 0,
    DIR_CCW = 1
} LED_Direction;

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
} Menu;

extern int8_t MenuIndex;
extern Menu *CurrentMenu;
extern uint8_t KeyPressed;

extern LED_Speed led_speed_value;
extern LED_Direction led_direction_value;

extern double pid_kp_value;
extern double pid_ki_value;
extern double pid_kd_value;

Menu *Init_Menu(void);
void displayMenu(void);
void displayMainMenu(void);
void handleMenuInput(void);

#endif
