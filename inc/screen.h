#ifndef KP_SCREEN_H
#define KP_SCREEN_H

//#include "menu.h"
#include "mygpiolib.h"
#include "timers.h"
#include "display.h"
#include "errors.h"
#include "lcd_hd44780.h"
//#include "intrinsics.h"

#include <stdint.h>
#include <stdio.h>

#define PASS_SHIFT 2

// enum kp_buttons{
//     KP_NONE     = 0,
//     KP_UP       = 1,
//     KP_DOWN     = 2,
//     KP_RIGHT    = 3,
//     KP_LEFT     = 4,
//     KP_MENU     = 5
// };

//
void kp_screen_empty(struct sk_lcd *lcd);
void kp_screen_input(struct sk_lcd *lcd, uint8_t passlength, char *instruction);
void kp_screen_timer(struct sk_lcd *lcd, uint32_t delay_s, uint8_t line);
void kp_screen_welcome(struct sk_lcd *lcd, uint32_t delay_s);

void kp_input_password(struct sk_lcd *lcd, uint8_t passlength, char *instruction);
void kp_fail(struct sk_lcd *lcd);
void kp_welcome(struct sk_lcd *lcd, bool mode);
//void kp_menu(struct sk_lcd *lcd);

#endif
