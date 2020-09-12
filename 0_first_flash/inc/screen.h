#ifndef KP_SCREEN_H
#define KP_SCREEN_H


#include "mygpiolib.h"
#include "timers.h"
#include "display.h"
#include "errors.h"
#include "lcd_hd44780.h"
#include <stdint.h>
#include <stdio.h>

#define PASS_SHIFT 2

void kp_screen_empty(struct sk_lcd *lcd);
void kp_screen_message(struct sk_lcd *lcd, char *part1, char *part2);

#endif
