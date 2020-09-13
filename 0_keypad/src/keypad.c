#include "keypad.h"

#define SEMIHOSTING_USE 0

#define GLOBAL_DATA_SIZE 9
#define ADRESS_FLASH 0x080E0000
//default keypad settings
//some of them might be changed by user in keypad menu
//Passwords
//To open/close the keypad
uint8_t USR_PASS_LENGTH = MIN_PASS_LENGTH;
uint8_t USR_PASS[MAX_PASS_LENGTH] = {1, 0, 3, 5};
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

// struct kp_lock keypad = {
//     .usrpass = {1, 0, 3, 5},
//     .mstrpass = {0, 0, 0, 0},
//     .menucode = {0, 0, 0, 0},
//     .delay_open_s = 10,
//     .delay_wait_s = 30,
//     .delay_wait_cur_s = 30,
//     .fails = 0,
//     .fails_low = 3,
//     .fails_high = 10,
//     .usrpass_length = 4,
//     .usrpass_length = 4,
//     .mode = true,
//     .state = false
// };

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
#endif

	//Write default Settings to FLASH
    //printf("-------------------Print DATA before");
//    print_data();
	//write_global_data_to_flash();

	WELCOME_DELAY_S = 30;
	//Read setting from flash
	read_global_data_from_flash();

#if SEMIHOSTING_USE
    printf("\n-------------------Print DATA after\n");
    //print_data();
#endif
	uint8_t pass[MAX_PASS_LENGTH]; //buffer for input pass

	mgl_clear(mgl_led_orange); //switch off indicator of settings

    while(1) {
		//sleep until user press on button
		__WFI();
		//always put pass to global array INPUT_PASS[MAX_PASS_LENGTH]
		kp_input_password(&lcd, &pass[0], USR_PASS_LENGTH, " Password", true);
		//check if user password
		if(kp_check_plain(USR_PASS, &pass[0], USR_PASS_LENGTH)){
			kp_welcome(&lcd, KP_MODE);
		}
		//check if menu code
		else if(kp_check_plain(MENU_CODE, &pass[0], USR_PASS_LENGTH)){
			kp_menu(&lcd);
		}
		else{
			kp_fail(&lcd);
		}
	}
}
