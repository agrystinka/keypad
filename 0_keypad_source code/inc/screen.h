#ifndef KP_SCREEN_H
#define KP_SCREEN_H

#include "menu.h"
#include "mygpiolib.h"
#include "timers.h"
#include "display.h"
#include "errors.h"
#include "lcd_hd44780.h"
#include <stdint.h>
#include <stdio.h>

#define PASS_SHIFT 2

void kp_screen_empty(struct sk_lcd *lcd);
void kp_screen_input(struct sk_lcd *lcd, uint8_t passlength, char *instruction);
void kp_screen_timer(struct sk_lcd *lcd, uint32_t delay_s, uint8_t line);
void kp_screen_welcome(struct sk_lcd *lcd, uint32_t delay_s);
void kp_screen_message(struct sk_lcd *lcd, char *part1, char *part2);

void kp_fail(struct sk_lcd *lcd);
void kp_welcome(struct sk_lcd *lcd, bool mode);

#endif
