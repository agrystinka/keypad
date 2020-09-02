#ifndef KP_KEYPAD_H
#define KP_KEYPAD_H

#include "setup.h"
#include "screen.h"
//#include "bl_bufflib.h"

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

#define KP_MENU 255

#define KP_LOCKED_STATE    0
#define KP_UNLOCKED_STATE  1
#define KP_BLOCKED_STATE   2
#define KP_SETTINGS_STATE  3

extern uint8_t kp_buff[8];
extern uint8_t kp_num;


#endif
