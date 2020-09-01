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

struct sk_lcd lcd = {
	.pin_group_data = &sk_io_lcd_data,
	.pin_rs = &sk_io_lcd_rs,
	.pin_en = &sk_io_lcd_en,
	.pin_rw = &sk_io_lcd_rw,
//	.pin_bkl = &sk_io_lcd_bkl,
	.set_backlight_func = timer1_set_pwm_backlight,
	.delay_func_us = NULL,
	.delay_func_ms = &delay_ms_systick,
	.is4bitinterface = true,
	.charmap_func = &sk_lcd_charmap_none
};




int main(void)
{
	rcc_periph_clock_enable(RCC_GPIOD);  //leds
	mgl_mode_setup_default(mgl_led_orange);
	mgl_set(mgl_led_orange); //indicator of settings

	rcc_periph_clock_enable(RCC_GPIOA); //bnt
	rcc_periph_clock_enable(RCC_GPIOC); //btn

	mgl_mode_setup_default(mgl_btn_swt1);
	mgl_mode_setup_default(mgl_btn_swt2);
	mgl_mode_setup_default(mgl_btn_swt3);
	mgl_mode_setup_default(mgl_btn_swt4);
	mgl_mode_setup_default(mgl_btn_swt5);

	// Configure interrupts
	//
	// // Set priority grouping. See p.228 Programming Manual for more
	// scb_set_priority_grouping(SCB_AIRCR_PRIGROUP_GROUP4_SUB4);	// default
	//
	// // Set priority for EXTI0.
	// const uint8_t group = 2;
	// const uint8_t subgroup = 0;
	// nvic_set_priority(NVIC_EXTI0_IRQ, (group << 2) | subgroup);
	//
	// // Enable EXTI port clock
	// rcc_periph_clock_enable(RCC_SYSCFG);
	//
	// exti_select_source(EXTI0, GPIOA);
	// exti_set_trigger(EXTI0, EXTI_TRIGGER_FALLING);	// ACTIVE HIGH, so trigger on 0->1 transition
	// exti_enable_request(EXTI0);
	// exti_reset_request(EXTI0);
	// nvic_enable_irq(NVIC_EXTI0_IRQ);
	//cm_enable_interrupts();

	// Configure lcd
	rcc_periph_clock_enable(RCC_GPIOE);		// lcd is connected to port E
	lcd_setup_default();
	mgl_pin_group_set(sk_io_lcd_data, 0x00);

	systick_setup();
	cm_enable_interrupts();

	init_bkl_pwm();
	sk_lcd_init(&lcd);
	sk_lcd_cmd_onoffctl(&lcd, true, false, false); // display on, cursor off, blinking off
	sk_lcd_set_backlight(&lcd, 200);

	lcd_custom_symb_load(&lcd);

	mgl_clear(mgl_led_orange); //indicator of settings

//------------------------------------------------
	lcd_print(&lcd, "Aa\t");

	lcd_print_symbol(&lcd, UP);
	lcd_print_symbol(&lcd, DOWN);
	lcd_print_symbol(&lcd, RIGHT);
	lcd_print_symbol(&lcd, LEFT);

	sk_lcd_cmd_setaddr(&lcd, 0x40, false);

	lcd_print_symbol(&lcd, POINT);
	lcd_print_symbol(&lcd, LOCKED);
	lcd_print_symbol(&lcd, UNLOCKED);
	lcd_print_symbol(&lcd, CLOSE);

    while(1) {
    }
}
