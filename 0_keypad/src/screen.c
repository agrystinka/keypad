#include "screen.h"
#include "keypad.h"

void kp_screen_empty(struct sk_lcd *lcd)
{
    sk_lcd_cmd_clear(lcd);
}

void kp_screen_message(struct sk_lcd *lcd, char *part1, char *part2)
{
    sk_lcd_cmd_clear(lcd);

    sk_lcd_cmd_setaddr(lcd, 0x00, false);
    if(part1 != NULL)
        lcd_print(lcd, part1);

    sk_lcd_cmd_setaddr(lcd, 0x40, false);
    if(part2 != NULL)
        lcd_print(lcd, part2);
}

void kp_screen_input(struct sk_lcd *lcd, uint8_t passlength, char *instruction)
{
    sk_lcd_cmd_clear(lcd);

    sk_lcd_cmd_setaddr(lcd, 0x00, false);
    lcd_print_symbol(lcd, STATE_SYMBOL);
    lcd_print(lcd, instruction);

    sk_lcd_cmd_setaddr(lcd, 0x40 + PASS_SHIFT, false);

    for(uint8_t i = 0;i < passlength; i++)
        lcd_print(lcd, "_");
}

void kp_screen_timer(struct sk_lcd *lcd, uint32_t delay_s, uint8_t line)
{
    //chose line
    uint8_t addr = 0x00;
    if(line != 0)
        addr = 0x40;
    //print timer
    for(int i = delay_s; i >= 0; i--){
        sk_lcd_cmd_setaddr(lcd, addr, false);
        //to print in centr
        lcd_print(lcd, "\t\t\t");
        lcd_print_time(lcd, i);
        delay_ms_systick(1000);
    }
}

void kp_fail(struct sk_lcd *lcd)
{
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
}

void kp_screen_welcome(struct sk_lcd *lcd, uint32_t delay_s)
{
    sk_lcd_cmd_clear(lcd);

    sk_lcd_cmd_setaddr(lcd, 0x00, false);
    lcd_print_symbol(lcd, UNLOCKED);
    lcd_print(lcd, " Welcome");

    kp_screen_timer(lcd, delay_s, 1);
}

void kp_unlock_keypad(struct sk_lcd *lcd)
{
    mgl_set(mgl_led_green);
    kp_screen_welcome(lcd, WELCOME_DELAY_S);
    mgl_clear(mgl_led_green);
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