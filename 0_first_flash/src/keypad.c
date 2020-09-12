#include "keypad.h"
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
// 
//
// void pack_global_data(uint32_t *buffer)
// {
// 	//uint32_t *pointer32 = buffer;
// 	//uint8_t *pointer8 = (uint8_t *) buffer;
// 	//pack delays  (uint32_t variables) 4 * 3 = 12 bytes
// 	buffer[0] = WELCOME_DELAY_S;
// 	buffer[1] = FAIL_DELAY_S;
// 	buffer[2] = FAIL_DELAY_CUR_S;
// 	//pack uint8_t variables
// 	//pack passwords: 2 * 8  = 16 bytes(if MAX_PASS_LENGTH == 8)
// 	uint8_t *pointer8 = (uint8_t *) &buffer[3];
// 	for(int i = 0; i < MAX_PASS_LENGTH; i++)
// 		*(pointer8 + i) = USR_PASS[i];
// 	pointer8 += MAX_PASS_LENGTH;
// 	for(int i = 0; i < MAX_PASS_LENGTH; i++)
// 		*(pointer8 + i) = MASTER_CODE[i];
// 	pointer8 += MAX_PASS_LENGTH;
// 	//pack pass kp_set_length: 2 bytes
// 	*(pointer8 + 0) = USR_PASS_LENGTH;
// 	*(pointer8 + 1) = MASTER_CODE_LENGTH;
// 	//pack settings: 5 bytes
// 	*(pointer8 + 2) = FAILS;
// 	*(pointer8 + 3) = CRYTICAL_FAILS_LOW;
// 	*(pointer8 + 4) = CRYTICAL_FAILS_HIGHT;
// 	*(pointer8 + 5) = DELAY_COEFF;
// 	if(KP_MODE)
// 		*(pointer8 + 6) = 1;
// 	else
// 		*(pointer8 + 6) = 0;
// 	//total: 12 + 16 + 2 + 5 = 35 byte = 9 words
// }
//
//
// void unpack_global_data(uint32_t *buffer)
// {
// 	//uint32_t *pointer32 = buffer;
// 	//uint8_t *pointer8 = (uint8_t *) buffer;
// 	//pack delays  (uint32_t variables) 4 * 3 = 12 bytes
// 	WELCOME_DELAY_S = buffer[0];
// 	FAIL_DELAY_S = buffer[1];
// 	FAIL_DELAY_CUR_S = buffer[2];
// 	//pack uint8_t variables
// 	//pack passwords: 2 * 8  = 16 bytes(if MAX_PASS_LENGTH == 8)
// 	uint8_t *pointer8 = (uint8_t *) &buffer[3];
// 	for(int i = 0; i < MAX_PASS_LENGTH; i++)
// 		USR_PASS[i] = *(pointer8 + i);
// 	pointer8 += MAX_PASS_LENGTH;
// 	for(int i = 0; i < MAX_PASS_LENGTH; i++)
// 		MASTER_CODE[i] = *(pointer8 + i);
// 	pointer8 += MAX_PASS_LENGTH;
// 	//pack pass kp_set_length: 2 bytes
// 	USR_PASS_LENGTH = *(pointer8 + 0);
// 	MASTER_CODE_LENGTH = *(pointer8 + 1);
// 	//pack settings: 5 bytes
// 	FAILS = *(pointer8 + 2);
// 	CRYTICAL_FAILS_LOW = *(pointer8 + 3);
// 	CRYTICAL_FAILS_HIGHT = *(pointer8 + 4);
// 	DELAY_COEFF = *(pointer8 + 5);
// 	if(*(pointer8 + 6))
// 		KP_MODE = true;
// 	else
// 		KP_MODE = false;
// 	//total: 12 + 16 + 2 + 5 = 35 byte = 9 words
// }
//
// void write_global_data_to_flash()
// {
// 	uint32_t data[9];
// 	pack_global_data(&data[0]);
// 	//instruction barrier is recomended
//
// 	//erase sector befor writing to it
// 	flash_erase_sector(15, 0);//sector - 0, program size - 8bit (0)
// 	//Check that no main Flash memory operation is ongoing
// 	//flash_wait_for_last_operation();
//
//
// 	//set PG to FLASH_CR,
// 	//perform the data write operation (32 bits)
// 	//wait for the BSY bit to be cleared.
// 	//flash_program_word(uint32_t address, uint32_t data);
//
// }

int main(void)
{
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

	//Write default Settings to FLASH
	//Read setting from flash

	uint8_t pass[MAX_PASS_LENGTH]; //buffer for input pass

	mgl_clear(mgl_led_orange); //switch off indicator of settings

    while(1) {
		//sleep until user press on button
		__WFI;
		//__asm__ volatile ("wfi");
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
