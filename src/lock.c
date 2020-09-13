#include "keypad.h"
#include "screen.h"
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


void kp_fail(struct sk_lcd *lcd, struct kp_lock *keypad)
{
	kp_btn_disable();
    keypad->fails++;

    //if(keypad->fails >= keypad->fails_hight){}
    //ask for ADM_PASS, red led on as signal tht smb tried to hack the keypad

    if(keypad->fails >= keypad->fails_low){
		sk_lcd_cmd_clear(lcd);
		kp_screen_fail(lcd);
		kp_screen_timer(lcd, keypad->delay_wait_cur_s, 1);
    	keypad->delay_wait_cur_s *= keypad->wait_coef;
	}
	kp_btn_enable();
}

void kp_unlock_keypad(struct sk_lcd *lcd, struct kp_lock *keypad)
{
	kp_btn_disable();
    mgl_set(mgl_led_green);
    kp_screen_welcome(lcd);
	kp_screen_timer(lcd, keypad->delay_open_s, 1);
    mgl_clear(mgl_led_green);
	kp_btn_enable();
}

void kp_toggle_keypad_state(struct kp_lock *keypad)
{
    if(STATE_SYMBOL == LOCKED)
        STATE_SYMBOL = UNLOCKED;
    else
        STATE_SYMBOL = LOCKED;

    //toggle door state
    mgl_toggle(mgl_led_green);
}

void kp_welcome(struct sk_lcd *lcd, struct kp_lock *keypad)
{
    //discard FAIL_DELAY
    keypad->fails = 0;
    keypad->delay_wait_cur_s = keypad->delay_wait_s;

    if(keypad->mode)
        kp_unlock_keypad(lcd, keypad);   //single action
    else
        kp_toggle_keypad_state(keypad); //switch state
}
