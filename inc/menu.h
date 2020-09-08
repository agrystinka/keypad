#ifndef KP_MENU_H
#define KP_MENU_H

//#include "keypad.h"
#include "mygpiolib.h"
#include "timers.h"
#include "display.h"
#include "errors.h"
#include "lcd_hd44780.h"
#include "intrinsics.h"

#include <stdint.h>
#include <stdio.h>

//reserve first coomand in menu as GO BACK
#define GO_BACK 0 

struct menu {
    uint8_t num;
    // uint8_t topline;
    // uint8_t activeline;
    char** lines;
    void (**options)(void);
};

// enum menu_lines {
//     KP_MENU_LINE0 = 0, //reserved as GO BACK
//     KP_MENU_LINE1,
//     KP_MENU_LINE2,
//     KP_MENU_LINE3,
//     KP_MENU_LINE4
// };

//void kp_screen_menu(struct sk_lcd *lcd);
void kp_menu(struct sk_lcd *lcd);
#endif
