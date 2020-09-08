#ifndef KP_KEYPAD_H
#define KP_KEYPAD_H

#include "keypad.h"
#include "screen.h"
#include "mygpiolib.h"
#include "timers.h"
#include "display.h"
#include "errors.h"
#include "lcd_hd44780.h"

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

bool kp_check_plain(uint8_t *password, uint8_t *input, uint8_t len);
void kp_input_password(struct sk_lcd *lcd, uint8_t passlength, char *instruction);

#endif
