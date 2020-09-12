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


void kp_fail(struct sk_lcd *lcd)
{
	kp_btn_disable();
    FAILS++;
    //if(CRYTICAL_FAILS >= FAILS){}
    //ask for ADM_PASS, red led on as signal tht smb tried to hack the keypad

    sk_lcd_cmd_clear(lcd);

    sk_lcd_cmd_setaddr(lcd, 0x00, false);
    lcd_print_symbol(lcd, STATE_SYMBOL);
    lcd_print(lcd, " Access denied");

    if(FAILS >= CRYTICAL_FAILS_LOW)
        kp_screen_timer(lcd, FAIL_DELAY_CUR_S, 1);

    FAIL_DELAY_CUR_S *= DELAY_COEFF;
	kp_btn_enable();
}

// void kp_screen_welcome(struct sk_lcd *lcd, uint32_t delay_s)
// {
//     sk_lcd_cmd_clear(lcd);
//
//     sk_lcd_cmd_setaddr(lcd, 0x00, false);
//     lcd_print_symbol(lcd, UNLOCKED);
//     lcd_print(lcd, " Welcome");
//
//     kp_screen_timer(lcd, delay_s, 1);
// }

void kp_unlock_keypad(struct sk_lcd *lcd)
{
	kp_btn_disable();
    mgl_set(mgl_led_green);
    kp_screen_welcome(lcd);
	kp_screen_timer(lcd, WELCOME_DELAY_S, 1);
    mgl_clear(mgl_led_green);
	kp_btn_enable();
}

void kp_toggle_keypad_state()
{
    if(STATE_SYMBOL == LOCKED)
        STATE_SYMBOL = UNLOCKED;
    else
        STATE_SYMBOL = LOCKED;

    //toggle door state
    mgl_toggle(mgl_led_green);
}

void kp_welcome(struct sk_lcd *lcd, bool mode)
{
    //discard FAIL_DELAY
    FAILS = 0;
    FAIL_DELAY_CUR_S = FAIL_DELAY_S;

    if(mode)
        kp_unlock_keypad(lcd);   //single action
    else
        kp_toggle_keypad_state(); //switch state
}
