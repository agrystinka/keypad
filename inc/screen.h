#ifndef KP_SCREEN_H
#define KP_SCREEN_H

#include "mygpiolib.h"
#include "timers.h"
#include "display.h"
#include "errors.h"
#include "lcd_hd44780.h"
#include "intrinsics.h"

#include <stdint.h>
#include <stdio.h>

enum kp_buttons{
    KP_NONE     = 0,
    KP_UP       = 1,
    KP_DOWN     = 2,
    KP_RIGHT    = 3,
    KP_LEFT     = 4,
    KP_MENU     = 5
};

extern uint8_t INPUT_PASS[MAX_PASS_LENGTH];
extern uint8_t INPUT_PASS_LENGTH;

void kp_screen_empty(struct sk_lcd *lcd);
void kp_screen_input(struct sk_lcd *lcd);
void kp_screen_timer(struct sk_lcd *lcd, uint32_t delay_s);
void kp_screen_welcome(struct sk_lcd *lcd);
void kp_screen_menu(struct sk_lcd *lcd);

void kp_print_insecure(struct sk_lcd *lcd, uint8_t cmd);
void kp_print_secure(struct sk_lcd *lcd);

void kp_input_password(struct sk_lcd *lcd)

#endif
