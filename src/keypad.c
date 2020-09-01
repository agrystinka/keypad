#include "setup.h"
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

//#define N_BTN 5
//const mgl_pin *buttons[5] = {&mgl_btn_swt1, &mgl_btn_swt2, &mgl_btn_swt3, &mgl_btn_swt4, &mgl_btn_swt5};
uint8_t PASS_LENGTH = 4;

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

//-------------------------------------------------------------------

bool sk_btn_debounce(mgl_pin btn)
{
	uint32_t btn_port = mgl_get_gpio_port(btn.port);
    uint32_t btn_pin = mgl_get_gpio_pin(btn.pin);

    //debouncing
    int cnt = 0;
    for(int i = 0; i < 10; i++)
        if(((gpio_port_read(btn_port) & btn_pin) == 0 && btn.inverse == 1) ||
           ((gpio_port_read(btn_port) & btn_pin) != 0 && btn.inverse == 0))
            cnt++;

    if(cnt >= 5){
        while(((gpio_port_read(btn_port) & btn_pin) == 0 && btn.inverse == 1) ||
           ((gpio_port_read(btn_port) & btn_pin) != 0 && btn.inverse == 0));
        return true;
    }
    else
        return false;
}

void exti15_10_isr(void)
{
	//check EXTI line
	if(exti_get_flag_status(EXTI15))
		if(sk_btn_debounce(mgl_btn_swt2)) //debouncing
			mgl_toggle(mgl_led_green);

	if(exti_get_flag_status(EXTI11))
		if(sk_btn_debounce(mgl_btn_swt1)) //debouncing
		mgl_toggle(mgl_led_green);
		// {	mgl_toggle(mgl_led_green);
		// 	sk_lcd_putchar(&lcd, 'a');
		// 	mgl_toggle(mgl_led_blue);
		// }
}

void exti9_5_isr(void)
{
	//check EXTI line
	if(exti_get_flag_status(EXTI6))
		if(sk_btn_debounce(mgl_btn_swt4)) //debouncing
			mgl_toggle(mgl_led_blue);
		//	lcd_print_symbol(&lcd, UP);

	else if(exti_get_flag_status(EXTI8))
		if(sk_btn_debounce(mgl_btn_swt5)) //debouncing
			//lcd_print_symbol(&lcd, DOWN);
			mgl_toggle(mgl_led_blue);

	else if(exti_get_flag_status(EXTI9))
		if(sk_btn_debounce(mgl_btn_swt3)) //debouncing
			mgl_toggle(mgl_led_blue);
			//	sk_lcd_putchar(&lcd, 'b');
			//	mgl_toggle(mgl_led_blue);
			//}
			//lcd_print_symbol(&lcd, LEFT);
}

int main(void)
{
	rcc_periph_clock_enable(RCC_GPIOD);  //leds
	mgl_mode_setup_default(mgl_led_orange);
	//mgl_set(mgl_led_orange); //indicator of settings
	mgl_mode_setup_default(mgl_led_green);
	//mgl_set(mgl_led_green); //indicator of settings
	mgl_mode_setup_default(mgl_led_blue);
	//mgl_set(mgl_led_blue); //indicator of settings

	mgl_mode_setup_default(mgl_led_red);
	//mgl_set(mgl_led_red); //indicator of settings

	kp_btn_setup();
	//mgl_clear(mgl_led_blue);

 	kp_interrupts_btn_setup(); 	// Configure interrupts
	//mgl_clear(mgl_led_red);

	kp_lcd_init_setup(&lcd); // Configure lcd

	systick_setup();
	cm_enable_interrupts();

	//set main lcd screen
	init_bkl_pwm();
	sk_lcd_init(&lcd);
	sk_lcd_cmd_onoffctl(&lcd, true, false, true); // display on, cursor off, blinking off
	sk_lcd_set_backlight(&lcd, 200);
	lcd_custom_symb_load(&lcd);

	mgl_clear(mgl_led_orange); //indicator of settings

//------------------------------------------------
	//lcd_print(&lcd, "Password\n");

	lcd_print_symbol(&lcd, LOCKED);
	sk_lcd_cmd_setaddr(&lcd, 0x40, false);
	for(int i = 0; i < PASS_LENGTH; i++)
		sk_lcd_putchar(&lcd, '_');
	//	lcd_print(&lcd, "_");
	sk_lcd_cmd_setaddr(&lcd, 0x40, false);
	//
	// lcd_print_symbol(&lcd, UP);
	// lcd_print_symbol(&lcd, DOWN);
	// lcd_print_symbol(&lcd, RIGHT);
	// lcd_print_symbol(&lcd, LEFT);
	//
	// sk_lcd_cmd_setaddr(&lcd, 0x40, false);
	//
	// lcd_print_symbol(&lcd, POINT);
	// lcd_print_symbol(&lcd, LOCKED);
	// lcd_print_symbol(&lcd, UNLOCKED);
	// lcd_print_symbol(&lcd, CLOSE);

    while(1) {
		__WFI();
    }
}
