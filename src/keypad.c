#include "keypad.h"
//#include "cmd.h"

//Passwords
//To open/close to keypad
uint8_t USR_PASS_LENGTH = MIN_PASS_LENGTH;
uint8_t USR_PASS[MAX_PASS_LENGTH] = {1, 2, 3, 4};
//To change settings
uint8_t MASTER_CODE_LENGTH = MIN_PASS_LENGTH;
uint8_t MASTER_CODE[MAX_PASS_LENGTH] = {0, 0, 0, 0}; //{5, 6, 7, 8};
//To open menu(settings)
uint8_t MENU_CODE_LENGTH = MIN_PASS_LENGTH;
uint8_t MENU_CODE[MAX_PASS_LENGTH] = {0, 0, 0, 0};
// //To store input pass
// uint8_t INPUT_PASS_LENGTH = 0;
// uint8_t INPUT_PASS[MAX_PASS_LENGTH];

//For comunication in interrupts
uint8_t KP_CMD = KP_NONE;

//Deleys and secure settings
uint32_t WELCOME_DELAY = MIN_DELAY_S;
uint32_t FAIL_DELAY = MIN_DELAY_S;
uint8_t FAILS = 0;
uint8_t CRYTICAL_FAILS = 10;
uint8_t DELAY_COEFF = 2;

//KEYPAD state
uint8_t STATE_SYMBOL = LOCKED;
bool KP_MODE = true;

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
	//mgl_mode_setup_default(mgl_led_green);
	// mgl_mode_setup_default(mgl_led_blue);
	// mgl_mode_setup_default(mgl_led_red);

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

	//Keypad simulation setup
	mgl_mode_setup_default(mgl_led_green);
	mgl_clear(mgl_led_green);

	mgl_clear(mgl_led_orange); //switch off indicator of settings

	//Keypad initial settiings, that might be changet in MENU

	uint8_t pass[MAX_PASS_LENGTH]; //buffer for input pass
    while(1) {
		__asm__ volatile ("wfi");
		//always put pass to global array INPUT_PASS[MAX_PASS_LENGTH]
		kp_input_password(&lcd, &pass[0], USR_PASS_LENGTH, " Password", true);
		//check if user password
		if(kp_check_plain(USR_PASS, &pass[0], USR_PASS_LENGTH)){
			kp_welcome(&lcd, KP_MODE);
		}
		//check if menu code
		else if(kp_check_plain(MENU_CODE, &pass[0], MENU_CODE_LENGTH)){
			kp_menu(&lcd);
		}
		else{
			kp_fail(&lcd);
		}
	}
}
