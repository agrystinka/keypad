#include "keypad.h"

uint8_t PASS_LENGTH = 4;
uint8_t PASS = 0;
uint8_t STATE = KP_LOCKED_STATE;

uint8_t kp_buff[8] = {0};

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
	mgl_mode_setup_default(mgl_led_green);
	mgl_mode_setup_default(mgl_led_blue);
	mgl_mode_setup_default(mgl_led_red);

	kp_btn_setup();
 	kp_interrupts_btn_setup(); 	// Configure interrupts

	kp_lcd_init_setup(&lcd); // Configure lcd

	systick_setup();
	cm_enable_interrupts();

	//set main lcd screen
	init_bkl_pwm();
	sk_lcd_init(&lcd);
	sk_lcd_cmd_onoffctl(&lcd, true, false, false); // display on, cursor off, blinking off
	sk_lcd_set_backlight(&lcd, 200);
	lcd_custom_symb_load(&lcd);

	mgl_clear(mgl_led_orange); //indicator of settings

	//kp_screen_timer(&lcd, 10);
	kp_screen_input(&lcd);

//	kp_screen_welcome(&lcd);

    while(1) {
		if(kp_num != 0){
			if(kp_buff[0] == KP_MENU)
				//menu 
				kp_screen_welcome(&lcd);
			else {
				PASS++;
				if(PASS < PASS_LENGTH)
					lcd_print_symbol(&lcd, POINT);
				else if(PASS == PASS_LENGTH) {
					lcd_print_symbol(&lcd, POINT);
					PASS = 0;
					//check if corrct
					kp_screen_welcome(&lcd);
				}
			}
			kp_num--;
		}
		__asm__ volatile ("wfi");
	}
}
