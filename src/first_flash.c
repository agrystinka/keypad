#include "keypad.h"
#include "embflash.h"
#include "screen.h"
#include "setup.h"
#include "cmd.h"

//if it is firt flash (loading default settings) it is true, else - false
bool FIRST_FLASH = true;
//For comunication in interrupts
uint8_t KP_CMD = KP_NONE;
//default setup as LOCKED
uint8_t STATE_SYMBOL = LOCKED; //LOCKED or UNLOCKED

struct kp_lock keypad = {
    .usrpass = {2, 2, 2, 2}, //up to 8 bytes
    .mstrpass = {0, 0, 0, 0}, //up to 8 bytes
    .menucode = {0, 0, 0, 0}, //up to 8 bytes
    .usrpass_length = 4,
    .mstrpass_length = 4,
    .delay_open_s = 10,
    .delay_wait_s = 30,
    .delay_wait_cur_s = 30,
    .fails = 0,
    .fails_low = 3,
    .wait_coef = 2,
    .mode = true,
    .state = false,
#if  HIGHT_SECURITY
    .semimstrpass_length = 4,
    .semimstrpass = {3, 3, 3, 3}, //up to 8 bytes
    .fails_high = 10,
#endif
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
void print_data(struct kp_lock *keypad);
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

    //kp_flash_init();
	kp_write_settings_to_flash(&keypad); //Write default setting to flash

#if SEMIHOSTING_USE
	// printf("Written data to flash\n");
	// print_data(&keypad);
	kp_read_settings_from_flash(&keypad);
	// printf("Read data from flash\n");
	// print_data(&keypad);
#endif

	kp_screen_message(&lcd, "Default data", "was set");
	mgl_clear(mgl_led_orange); //switch off indicator of settings

    while(1);
}

#if SEMIHOSTING_USE
void print_data(struct kp_lock *keypad)
{
    printf("User pass length: %d\n", keypad->usrpass_length);
    printf("Master pass length: %d\n", keypad->mstrpass_length);

    printf("Delay open: %lu\n", keypad->delay_open_s);
    printf("Delay close: %lu\n", keypad->delay_wait_s);
    printf("Delay close cur: %lu\n", keypad->delay_wait_cur_s);

    printf("Fails: %d\n", keypad->fails);
    printf("Fails low: %d\n", keypad->fails_low);
    printf("Fails coefficient: %d\n", keypad->wait_coef);

    if(keypad->mode)
        printf("Mode TRUE\n");
    else
        printf("Mode FALSE\n");

    if(keypad->mode)
        printf("State TRUE\n");
    else
        printf("State FALSE\n");

#if  HIGHT_SECURITY
    printf("Semimaster pass length: %d\n", keypad->semimstrpass_length);
    printf("Fails high: %d\n", keypad->fails_high);
#endif
}
#endif
