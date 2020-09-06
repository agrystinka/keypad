#include "keypad.h"

//Passwords
uint8_t USR_PASS_LENGTH = MIN_PASS_LENGTH;
uint8_t USR_PASS[MAX_PASS_LENGTH];

uint8_t ADM_PASS_LENGTH = MIN_PASS_LENGTH;
uint8_t ADM_PASS[MAX_PASS_LENGTH];

uint8_t MENU_CODE_LENGTH = MIN_PASS_LENGTH;
uint8_t MENU_CODE[MAX_PASS_LENGTH];

uint8_t INPUT_PASS_LENGTH = 0;
uint8_t INPUT_PASS[MAX_PASS_LENGTH];
// //Passwords
// uint8_t USR_PASS_LENGTH = MIN_PASS_LENGTH;
// uint8_t USR_PASS[MAX_PASS_LENGTH] = {1, 2, 3, 4};
//
// uint8_t ADM_PASS_LENGTH = MIN_PASS_LENGTH;
// uint8_t ADM_PASS[MAX_PASS_LENGTH] = {5, 6, 7, 8};
//
// uint8_t MENU_CODE_LENGTH = MIN_PASS_LENGTH;
// uint8_t MENU_CODE[MAX_PASS_LENGTH] = {0, 0, 0, 0};
//
// uint8_t INPUT_PASS_LENGTH = 0;
// uint8_t INPUT_PASS[MAX_PASS_LENGTH];

uint8_t KP_CMD = KP_NONE;

//display
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

// void kp_btn_enable(void)
// {
// 	nvic_enable_irq(NVIC_EXTI15_10_IRQ);
// 	nvic_enable_irq(NVIC_EXTI9_5_IRQ);
// }
//
// void kp_btn_disable(void)
// {
// 	nvic_disable_irq(NVIC_EXTI15_10_IRQ);
// 	nvic_disable_irq(NVIC_EXTI9_5_IRQ);
// }

bool kp_check_plain(uint8_t *password, uint8_t *input, uint8_t len)
{
    for(uint8_t i = 0; i < len; i++)
       if(password[i] != input[i])
           return false;
    return true;
}

int main(void)
{
	//enable leds
	rcc_periph_clock_enable(RCC_GPIOD);  //leds
	mgl_mode_setup_default(mgl_led_orange);

	mgl_set(mgl_led_orange); //switch on indicator of settings

	kp_btn_setup(); // Setup btns on GLSK board
 	kp_interrupts_btn_setup(); 	// Configure exti on GLSK buttons

	kp_lcd_init_setup(&lcd); // Configure lcd

	systick_setup();
	cm_enable_interrupts();

	//set main lcd screen
	init_bkl_pwm();
	sk_lcd_init(&lcd);
	sk_lcd_cmd_onoffctl(&lcd, true, false, false); // display on, cursor off, blinking off
	sk_lcd_set_backlight(&lcd, 200);
	lcd_custom_symb_load(&lcd);

	mgl_clear(mgl_led_orange); //switch off indicator of settings

    while(1) {
		kp_input_password(&lcd);
		//check if meny code
		if(kp_check_plain(MENU_CODE, INPUT_PASS, MENU_CODE_LENGTH)){
			//menu

		}
		//check if user password
		else if(kp_check_plain(USR_PASS, INPUT_PASS, USR_PASS_LENGTH)){

			//action
		}
		else{
			//access denied
		}
	}
}
