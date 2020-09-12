#include "first_flash.h"
//#include "cmd.h"

//default keypad settings
//some of them might be changed by user in keypad menu
//Passwords
//To open/close the keypad
uint8_t USR_PASS_LENGTH = MIN_PASS_LENGTH;
uint8_t USR_PASS[MAX_PASS_LENGTH] = {1, 2, 3, 4};
//To change keypad settings
uint8_t MASTER_CODE_LENGTH = MIN_PASS_LENGTH;
uint8_t MASTER_CODE[MAX_PASS_LENGTH] = {0, 0, 0, 0}; //{5, 6, 7, 8};
//To open menu(keypad settings)
uint8_t MENU_CODE_LENGTH = MIN_PASS_LENGTH;
uint8_t MENU_CODE[MAX_PASS_LENGTH] = {0};

//For comunication in interrupts
uint8_t KP_CMD = KP_NONE;

//Deleys and secure settings
uint32_t WELCOME_DELAY_S = 10;
uint32_t FAIL_DELAY_S = 30;
uint32_t FAIL_DELAY_CUR_S = 30;
uint8_t FAILS = 0;
uint8_t CRYTICAL_FAILS_LOW = 3;
uint8_t CRYTICAL_FAILS_HIGHT = 10;
uint8_t DELAY_COEFF = 2;

//KEYPAD state
//default setup as LOCKED
uint8_t STATE_SYMBOL = LOCKED; //LOCKED or UNLOCKED
//if true - unlock keypad for WELCOME_DELAY seconds
//if false -switch the STATE of keypad
bool KP_MODE = true;
bool DEFAULT_SETTINGS = true;
//uint32_t GLOBAL_DATA[20];

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

void kp_btn_enable(void)
{
	nvic_enable_irq(NVIC_EXTI15_10_IRQ);
	nvic_enable_irq(NVIC_EXTI9_5_IRQ);
}

void kp_btn_disable(void)
{
	nvic_disable_irq(NVIC_EXTI15_10_IRQ);
	nvic_disable_irq(NVIC_EXTI9_5_IRQ);
}


int main(void)
{
	///enable leds
	rcc_periph_clock_enable(RCC_GPIOD);  //leds
	mgl_mode_setup_default(mgl_led_orange);
	mgl_set(mgl_led_orange); //switch on indicator of settings

	kp_lcd_init_setup(&lcd); // Configure lcd

	systick_setup();
	cm_enable_interrupts();

	//set main lcd screen
	init_bkl_pwm();
	sk_lcd_init(&lcd);
	sk_lcd_cmd_onoffctl(&lcd, true, false, false); // display on, cursor off, blinking off
	sk_lcd_set_backlight(&lcd, 200);
	lcd_custom_symb_load(&lcd); //load spesial custom symbols to show the keypad state

	//Write default Settings to FLASH
	write_global_data_to_flash();
	//Show message to user
	kp_screen_message(&lcd, "Default data", "was set");

	mgl_clear(mgl_led_orange); //switch off indicator of settings

    while(1);
}
