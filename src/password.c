#include "password.h"
#include "intrinsics.h"
#include "cmd.h"
#include "screen.h"

bool kp_check_plain(uint8_t *pass1, uint8_t *pass2, uint8_t length)
{
    for(uint8_t i = 0; i < length; i++)
       if(pass1[i] != pass2[i])
           return false;
    return true;
}


void kp_input_password(struct sk_lcd *lcd, uint8_t *pass, uint8_t passlength, char *instruction, bool secure)
{
    //fill password with zero, it is entry state
    for(int i = 0; i < passlength; i++)
        pass[i] = 0;

    //index of input number
    int8_t curnum = 0;
	//show input password screen
    kp_screen_input(lcd, passlength, instruction);

    while(1){
        //sleep untill interrupt occurs
		__WFI;
        //if interrupt occured
        if(KP_CMD != KP_NONE){
            if(KP_CMD == KP_MENU){
                KP_CMD = KP_NONE;
                return;
            }
            else{
                if(KP_CMD == KP_UP)
                    pass[curnum] = (pass[curnum] + 1) % 10;
                else if(KP_CMD == KP_DOWN)
                    pass[curnum] = (pass[curnum] - 1 + 10) % 10;
                else{
                    //Hide previous symbol
                    if(secure){
                        sk_lcd_cmd_setaddr(lcd, 0x40 + PASS_SHIFT + curnum, false);
                        lcd_print_symbol(lcd, POINT);
                    }
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
                //Refresh output on display
                sk_lcd_cmd_setaddr(lcd, 0x40 + PASS_SHIFT + curnum, false);
                lcd_print_int(lcd, pass[curnum], 0);
                KP_CMD = KP_NONE;
            }
        }
    }
}
