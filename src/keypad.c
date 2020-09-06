#include "keypad.h"

//Password
uint8_t PASS[MAX_PASS_LENGTH];
uint8_t PASS_LENGTH;

uint8_t INPUT_PASS[MAX_PASS_LENGTH];
uint8_t INPUT_PASS_LENGTH = 0;

//Input buffer
bl_bfifo_t kp_fifo_buff;
uint8_t kp_buff[MAX_PASS_LENGTH]; //for kp_fifo_buff initialization

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

bool kp_check_plain(uint8_t *password, uint8_t *input, uint8_t len)
{
    for(uint8_t i = 0; i < len; i++)
       if(password[i] != input[i])
           return false;
    return true;
}

void kp_catch_error(void)
{
	//an error occured, restart pasword input
	INPUT_PASS_LENGTH = 0; //discard input password
	kp_screen_input(&lcd);
}

int main(void)
{
	//enable leds
	rcc_periph_clock_enable(RCC_GPIOD);  //leds
	mgl_mode_setup_default(mgl_led_orange);
	// mgl_mode_setup_default(mgl_led_green);
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

	//Initialization of input buffer
	bl_bfifo_init(&kp_fifo_buff, &kp_buff[0], MAX_PASS_LENGTH);
	PASS[0] = KP_UP;
	PASS[1] = KP_UP;
	PASS[2] = KP_UP;
	PASS[3] = KP_DOWN;
	PASS_LENGTH = MIN_PASS_LENGTH;

	mgl_clear(mgl_led_orange); //switch off indicator of settings

	//Show input screen, keypad in in LOCKED mode
	kp_screen_input(&lcd);

    while(1) {
		//sleep untill interrupt occurs
		//TO DO : wrapp it
		__asm__ volatile ("wfi");

		if(KP_CMD != KP_NONE){
			if(KP_CMD == KP_MENU){
				KP_CMD = KP_NONE;
				KP_MENU_ACTIVE = 1;
				INPUT_PASS_LENGTH = 0;//discard input password
				kp_screen_menu(&lcd);
				KP_MENU_ACTIVE = 0;
			}
			else {
				//disable exti btn interrupts
				INPUT_PASS[INPUT_PASS_LENGTH] = KP_CMD;
				INPUT_PASS_LENGTH++;
				kp_print_insecure(&lcd, KP_CMD);

				//if PASSWORD was written
				if(INPUT_PASS_LENGTH == PASS_LENGTH) {
					//block buttons
					kp_btn_disable();
					//check if password is correct
					if (kp_check_plain(&PASS[0], &INPUT_PASS[0], PASS_LENGTH)){ //open door
						kp_screen_welcome(&lcd);
					}
					else{
						kp_screen_timer(&lcd, 10);    //delay for n s
					}
					INPUT_PASS_LENGTH = 0; //discard input password
					//enable interrupts
					kp_btn_enable();
					kp_screen_input(&lcd);
				}
				else if(INPUT_PASS_LENGTH > PASS_LENGTH){
					//an error occured, restart pasword input
					INPUT_PASS_LENGTH = 0; //discard input password
					kp_screen_input(&lcd);
				}
				//enable exti btn interrupts
			}

			KP_CMD = KP_NONE;
		}
	}
}
