#include <libopencm3/cm3/cortex.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/scb.h>

static void kp_btn_enable(void)
{
	nvic_enable_irq(NVIC_EXTI15_10_IRQ);
	nvic_enable_irq(NVIC_EXTI9_5_IRQ);
}

static void kp_btn_disable(void)
{
	nvic_disable_irq(NVIC_EXTI15_10_IRQ);
	nvic_disable_irq(NVIC_EXTI9_5_IRQ);
}
void kp_fail(struct sk_lcd *lcd)
{
    FAILS++;
    //if(CRYTICAL_FAILS >= FAILS){}
    //ask for ADM_PASS, red led on as signal tht smb tried to hack the keypad

    sk_lcd_cmd_clear(lcd);

    sk_lcd_cmd_setaddr(lcd, 0x00, false);
    lcd_print_symbol(lcd, STATE_SYMBOLA);
    lcd_print(lcd, " Access denied");

    if(FAILS >= CRYTICAL_FAILS_LOW)
        kp_screen_timer(lcd, FAIL_DELAY, 1);

    FAIL_DELAY *= DELAY_COEFF;
}


void kp_welcome(struct sk_lcd *lcd, bool mode)
{
    //discard FAIL_DELAY
    FAILS = 0;
    FAIL_DELAY = MIN_DELAY_S;

    //single action
    if(mode){
        mgl_set(mgl_led_green);
        kp_screen_welcome(lcd, WELCOME_DELAY);
        mgl_clear(mgl_led_green);
    }
    //switch state
    else{
        if(STATE_SYMBOL == LOCKED)
            STATE_SYMBOL = UNLOCKED;
        else
            STATE_SYMBOL = LOCKED;
        mgl_toggle(mgl_led_green);
    }
}
