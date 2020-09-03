#ifndef KP_KEYPAD_H
#define KP_KEYPAD_H

#include "setup.h"
#include "screen.h"
#include "bl_bufflib.h"

#include "mygpiolib.h"
#include "timers.h"
#include "display.h"
#include "errors.h"
#include "lcd_hd44780.h"
#include "intrinsics.h"

#include <libopencm3/cm3/cortex.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/scb.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>

#define KP_MENU 0xff

#define MAX_PASS_LENGTH 8
#define MIN_PASS_LENGTH 4

#define KP_LOCKED_STATE    0
#define KP_UNLOCKED_STATE  1
#define KP_BLOCKED_STATE   2
#define KP_SETTINGS_STATE  3

extern uint8_t PASS[MAX_PASS_LENGTH];
extern uint8_t PASS_LENGTH;

extern uint8_t INPUT_PASS[MAX_PASS_LENGTH];
extern uint8_t INPUT_PASS_LENGTH;

extern bl_bfifo_t kp_fifo_buff;



//bool kp_check_plain(uint8_t *pass)

#endif
