#ifndef KP_SETUP_H
#define KP_SETUP_H

#include "mygpiolib.h"
#include "timers.h"
#include "display.h"
#include "errors.h"
#include "lcd_hd44780.h"
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

sk_err kp_lcd_init_setup(struct sk_lcd *lcd);
//sk_err kp_lcd_setup(struct sk_lcd *lcd);
sk_err kp_btn_setup(void);
sk_err kp_interrupts_btn_setup(void);
// sk_err kp_flash_setup(void);
#endif
