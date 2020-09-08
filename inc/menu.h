#ifndef KP_MENU_H
#define KP_MENU_H

#include "keypad.h"
#include "mygpiolib.h"
#include "timers.h"
#include "display.h"
#include "errors.h"
#include "lcd_hd44780.h"
#include "intrinsics.h"

#include <stdint.h>
#include <stdio.h>

struct menu {
    uint8_t num;
    // uint8_t topline;
    // uint8_t activeline;
    char** lines;
    void (**options)(struct sk_lcd *lcd);
};

void kp_menu(struct sk_lcd *lcd);
#endif
