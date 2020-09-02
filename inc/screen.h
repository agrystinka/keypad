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

extern uint8_t PASS_LENGTH;

void kp_screen_empty(struct sk_lcd *lcd);
void kp_screen_input(struct sk_lcd *lcd);
void kp_screen_timer(struct sk_lcd *lcd, uint32_t delay_s);
void kp_screen_welcome(struct sk_lcd *lcd);

#endif
