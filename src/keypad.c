#include "keypad.h"

#define SEMIHOSTING_USE 0
#define HIGHT_SECURITY 0

//For comunication in interrupts
uint8_t KP_CMD = KP_NONE;
//default setup as LOCKED
uint8_t STATE_SYMBOL = LOCKED; //LOCKED or UNLOCKED

struct kp_lock keypad = {
    .usrpass = {2, 2, 2, 2}, //up to 8 bytes
    .mstrpass = {0, 0, 0, 0}, //up to 8 bytes
    .menucode = {0, 0, 0, 0}, //up to 8 bytes
    .delay_open_s = 10,
    .delay_wait_s = 30,
    .delay_wait_cur_s = 30,
	.wait_coef = 2,
    .fails = 0,
    .fails_low = 3,
    .fails_high = 10,
    .usrpass_length = 4,
    .mstrpass_length = 4,
    .mode = true,
    .state = false
};

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

#if SEMIHOSTING_USE
void print_data(void)
{
	printf("USER PASS LENGHT %x\n", USR_PASS_LENGTH);
	for(int i =0 ; i < USR_PASS_LENGTH; i++)
		printf("%x   ", USR_PASS[i]);
	printf("\n");
	printf("MASTER PASS LENGHT %x\n", MASTER_CODE_LENGTH);
	for(int i =0 ; i < MASTER_CODE_LENGTH; i++)
		printf("%x   ", MASTER_CODE[i]);
	printf("\n");
	printf("WELCOME_DELAY_S %x\n",  WELCOME_DELAY_S);
	printf("FAIL_DELAY_S %x\n", FAIL_DELAY_S);
	printf("FAIL_DELAY_CUR_S %x\n", FAIL_DELAY_CUR_S);

	printf("FAILS %x\n",  FAILS);
	printf("CRYTICAL_FAILS_LOW %x\n",  CRYTICAL_FAILS_LOW);
	printf("CRYTICAL_FAILS_HIGHT %x\n", CRYTICAL_FAILS_HIGHT);
	printf("DELAY_COEFF %x\n",  DELAY_COEFF);

	if(KP_MODE)
		printf("KP_MODE true\n");
	else
		printf("KP_MODE false\n");

	printf("\n");
	printf("\n");

}
#endif

int main(void)
{
#if SEMIHOSTING_USE
    initialise_monitor_handles();
    printf("Monitor initialized. WE GET IT THROUGH SEMIHOSTING\n");
#endif
	///enable leds
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
	lcd_custom_symb_load(&lcd); //load spesial custom symbols to show the keypad state

	//Keypad simulation setup
	//For real keypad it must be changed with setup of solenoid, etc.
	mgl_mode_setup_default(mgl_led_green);
	//setup as LOCKED
	mgl_clear(mgl_led_green);

#if SEMIHOSTING_USE
    printf("System initialized\n");
    printf("Load data from flash\n");
#endif

	write_keypad_data_to_flash(&keypad);

	//Read setting from flash
	//read_keypad_data_from_flash(&keypad);

#if SEMIHOSTING_USE
    printf("\n-------------------Print DATA after\n");
    //print_data();
#endif
	uint8_t pass[MAX_PASS_LENGTH]; //buffer for input pass

	mgl_clear(mgl_led_orange); //switch off indicator of settings

    //if there were failed attempts to unlock keypad before reset
    //wait for delay_wait_cur_s seconds
    if(keypad.fails > 0){
        //but do not calculate it like one more fail
        keypad.fails--;
        keypad.delay_wait_cur_s /= keypad.wait_coef;
        kp_fail(&lcd, &keypad);
    }



    while(1) {
		//sleep until user press on button
		__WFI();
		//always put pass to global array INPUT_PASS[MAX_PASS_LENGTH]
		kp_input_password(&lcd, &pass[0], keypad.usrpass_length, " Password", true);
		//check if user password
		if(kp_check_plain(keypad.usrpass, &pass[0], keypad.usrpass_length)){
			kp_welcome(&lcd, &keypad);
		}
		//check if menu code
		else if(kp_check_plain(keypad.menucode, &pass[0], keypad.usrpass_length)){
			kp_menu(&lcd, &keypad);
		}
		else{
			kp_fail(&lcd, &keypad);
		}
	}
}
