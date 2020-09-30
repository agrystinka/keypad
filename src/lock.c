#include "lock.h"
#include "embflash.h"
#include <libopencm3/cm3/cortex.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/scb.h>

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


#if HIGHT_SECURITY
//TODO: test properly
static void kp_fail_crit_hight(struct sk_lcd *lcd, struct kp_lock *keypad)
{
	uint8_t pass[MAX_PASS_LENGTH]; //buffer for input pass
	kp_btn_enable();
	kp_input_password(&lcd, &pass[0], keypad->semimstrpass_length, " Semimaster pass", true);
	kp_btn_disable();
	kp_write_logs_to_flash(&keypad);
	if(kp_check_plain(keypad->semimstrpass, &pass[0], keypad->semimstrpass_length)){
		kp_logs_in_flash_successed();
		kp_discard_fails(keypad);//discard FAIL_DELAY
	}else{
		sk_lcd_cmd_clear(lcd);
		kp_screen_fail(lcd);
		kp_screen_timer(lcd, keypad->delay_wait_cur_s, 1);
		//TODO: Set timer with control of overload
		keypad->fails++;
		keypad->delay_wait_cur_s *= keypad->wait_coef;
	}
	kp_btn_enable();
	return;
}
#endif



void kp_fail(struct sk_lcd *lcd, struct kp_lock *keypad, bool count)
{
	kp_btn_disable();

#if HIGHT_SECURITY
	if(keypad->fails >= keypad->fails_high){
		kp_fail_crit_hight(lcd, keypad);
		return;
	}
#endif

	if(count)
		keypad->fails++;

    if(keypad->fails >= keypad->fails_low ){
		sk_lcd_cmd_clear(lcd);
		kp_screen_fail(lcd);
		kp_screen_timer(lcd, keypad->delay_wait_cur_s, 1);
		if(count)
    		keypad->delay_wait_cur_s *= keypad->wait_coef;
	}
	kp_logs_in_flash_failed_p();
	kp_btn_enable();
}

void kp_lock_keypad(struct kp_lock *keypad)
{
	//simuletion of solenoid
    mgl_clear(mgl_led_green);
	keypad->state = false;
}

void kp_unlock_keypad(struct kp_lock *keypad)
{
	//simulation of solenoid
    mgl_set(mgl_led_green);
	keypad->state = true;
}

void kp_single_opening(struct sk_lcd *lcd, struct kp_lock *keypad)
{
	kp_btn_disable(); //block buttons just in case
	kp_unlock_keypad(keypad);
    kp_screen_welcome(lcd);
	kp_screen_timer(lcd, keypad->delay_open_s, 1);
	kp_lock_keypad(keypad);
	kp_btn_enable();
}

void kp_toggle_keypad_state(struct kp_lock *keypad)
{
	//if keypad is locked, unlock it and vise versa
    if(keypad->state){
		kp_lock_keypad(keypad);
		STATE_SYMBOL = LOCKED;
	}else{
		kp_unlock_keypad(keypad);
		STATE_SYMBOL = UNLOCKED;
	}
}

void kp_discard_fails(struct kp_lock *keypad)
{
	keypad->fails = 0;
	keypad->delay_wait_cur_s = keypad->delay_wait_s;
}

void kp_welcome(struct sk_lcd *lcd, struct kp_lock *keypad)
{
	kp_btn_disable(); //just in case

    kp_discard_fails(keypad);//discard FAIL_DELAY

    if(keypad->mode)
        kp_single_opening(lcd, keypad);   //single action
    else
        kp_toggle_keypad_state(keypad); //switch state

	kp_btn_enable();
}
