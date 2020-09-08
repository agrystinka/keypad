#include "screen.h"
#include "keypad.h"

// #define PASS_SHIFT 2

void kp_screen_empty(struct sk_lcd *lcd)
{
    sk_lcd_cmd_clear(lcd);
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
//
void kp_input_password(struct sk_lcd *lcd, uint8_t passlength, char *instruction)
{
    //fill password with zero, it is entry state
    for(int i = 0; i < passlength; i++)
        INPUT_PASS[i] = 0;

    //index of input number
    uint8_t curnum = 0;
	//show input password screen
    kp_screen_input(lcd, passlength, instruction);

    while(1){
        __asm__ volatile ("wfi");
        if(KP_CMD != KP_NONE){
            if(KP_CMD == KP_MENU){
                KP_CMD = KP_NONE;
                return;
            }
            else{
                if(KP_CMD == KP_UP)
                    INPUT_PASS[curnum] = (INPUT_PASS[curnum] + 1) % 10;
                else if(KP_CMD == KP_DOWN)
                    INPUT_PASS[curnum] = (INPUT_PASS[curnum] - 1 + 10) % 10;
                else{
                    //Hide previous symbol
                    sk_lcd_cmd_setaddr(lcd, 0x40 + PASS_SHIFT + curnum, false);
                    lcd_print_symbol(lcd, POINT);

                    //Change position
                    if(KP_CMD == KP_RIGHT)
                        curnum++;
                    else if(KP_CMD == KP_LEFT)
                        curnum--;
                    //Barriers
                    if(curnum < 0)
                        curnum = 0;
                    else if(curnum >= passlength)
                        curnum = passlength - 1;
                }
                sk_lcd_cmd_setaddr(lcd, 0x40 + PASS_SHIFT + curnum, false);
                lcd_print_int(lcd, INPUT_PASS[curnum], 0);
                KP_CMD = KP_NONE;
            }
        }
    }
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
    //double FAIL_DELAY
    FAILS++;
    //FAIL_DELAY = 130;
    //if(CRYTICAL_FAILS == FAILS){}
    //ask for ADM_PASS, red led on as signal tht smb tried to hack the keypad

    sk_lcd_cmd_clear(lcd);

    sk_lcd_cmd_setaddr(lcd, 0x00, false);
    lcd_print_symbol(lcd, LOCKED);
    lcd_print(lcd, " Access denied");

    kp_screen_timer(lcd, FAIL_DELAY, 1);

    FAIL_DELAY *= DELAY_COEFF;
}


void kp_screen_welcome(struct sk_lcd *lcd, uint32_t delay_s)
{
    sk_lcd_cmd_clear(lcd);

    sk_lcd_cmd_setaddr(lcd, 0x00, false);
    lcd_print_symbol(lcd, UNLOCKED);
    lcd_print(lcd, " Welcome");

    kp_screen_timer(lcd, delay_s, 1);
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
//-----------------------------------------------------------------------------------------------------------
// void kp_main_settings(struct sk_lcd *lcd)
// {
//
// }
//
//
// void kp_menu(struct sk_lcd *lcd)
// {
//     kp_input_password(lcd, MASTER_CODE_LENGTH, " Master code");
//     //check if correct MASTER_CODE
//     if(kp_check_plain(INPUT_PASS, MASTER_CODE, MASTER_CODE_LENGTH)){
//         //access to settings
//     }
//     else{
//         //access denied
//         //wait 2 min untill next MASTER_CODE input will be allowed
//         //go back to user pass input
//
//     }
// }
