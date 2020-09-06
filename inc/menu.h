#ifndef KP_MENU_H
#define KP_MENU_H

#include "screen.h"
#include "mygpiolib.h"
#include "timers.h"
#include "display.h"
#include "errors.h"
#include "lcd_hd44780.h"
#include "intrinsics.h"

#include <stdint.h>
#include <stdio.h>

extern uint8_t KP_MENU_ACTIVE;

enum menu_main_cmd {
    GO_BACK = 0,
    WELCOME_TIMER = 1,
    DENY_TIMER = 2,
    CHANGE_PASS = 3
};



void kp_screen_menu(struct sk_lcd *lcd);


#endif
