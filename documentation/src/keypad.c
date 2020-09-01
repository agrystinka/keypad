#include "setup.h"
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

void exti15_10_isr 	(void)
{
	//check EXTI line
	if(exti_get_flag_status(EXTI15)){
		//debouncing
		if(sk_btn_debounce(mgl_btn_swt2))
			//switch light
			mgl_toggle(mgl_led_green);
}


int main(void)
{
	rcc_periph_clock_enable(RCC_GPIOD);  //leds
	mgl_mode_setup_default(mgl_led_orange);
	mgl_set(mgl_led_orange); //indicator of settings
	mgl_mode_setup_default(mgl_led_green);
	mgl_set(mgl_led_green); //indicator of settings

	kp_btn_setup();
 	kp_interrupts_btn_setup(); 	// Configure interrupts
	
	kp_lcd_setup(&lcd); // Configure lcd
	cm_enable_interrupts();

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
