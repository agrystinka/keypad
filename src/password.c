#include "password.h"

bool kp_check_plain(uint8_t *password, uint8_t *input, uint8_t len)
{
    for(uint8_t i = 0; i < len; i++)
       if(password[i] != input[i])
           return false;
    return true;
}


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
