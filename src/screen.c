#include "screen.h"
#include "keypad.h"

#define PASS_SHIFT 2

void kp_screen_empty(struct sk_lcd *lcd)
{
    sk_lcd_cmd_clear(lcd);
}

void kp_screen_input(struct sk_lcd *lcd)
{
    sk_lcd_cmd_clear(lcd);

    sk_lcd_cmd_setaddr(lcd, 0x00, false);
    lcd_print_symbol(lcd, LOCKED);
    lcd_print(lcd, " Password:");

    sk_lcd_cmd_setaddr(lcd, 0x40 + PASS_SHIFT, false);

    for(uint8_t i = 0;i < PASS_LENGTH; i++)
        //lcd_print(lcd, "0");
        lcd_print(lcd, "_");
}

void kp_input_password(struct sk_lcd *lcd)
{
    //fill password with zero, it is entry state
    for(int i = 0; i < passlength; i++)
        pass[i] = 0;
    //index of input number
    uint8_t curnum = 0;
    kp_screen_input(&lcd);

    while(1){
        __asm__ volatile ("wfi");
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
                lcd_print_int(lcd, pass[curnum], 0);
                KP_CMD = KP_NONE;
            }
        }
    }
}



void kp_screen_timer(struct sk_lcd *lcd, uint32_t delay_s)
{
    //deny interrupts
    sk_lcd_cmd_clear(lcd);

    sk_lcd_cmd_setaddr(lcd, 0x00, false);
    lcd_print_symbol(lcd, LOCKED);
    lcd_print(lcd, " Access denied");

    for(int i = delay_s; i >= 0; i--){
        lcd_print_time(lcd, i);
        delay_ms_systick(1000);
    }
}

void kp_screen_welcome(struct sk_lcd *lcd)
{
    sk_lcd_cmd_clear(lcd);

    sk_lcd_cmd_setaddr(lcd, 0x00, false);
    lcd_print_symbol(lcd, UNLOCKED);
    lcd_print(lcd, " Welcome");

    uint32_t delay_s = 10;
    for(int i = delay_s; i >= 0; i--){
        lcd_print_time(lcd, i);
        delay_ms_systick(1000);
    }
}



void kp_print_insecure(struct sk_lcd *lcd, uint8_t cmd)
{
    uint8_t symbol = 0xFF; //black squre
    switch(cmd){
        case KP_UP    : symbol = UP; break;
        case KP_DOWN  : symbol = DOWN; break;
        case KP_RIGHT : symbol = RIGHT; break;
        case KP_LEFT  : symbol = LEFT; break;
        case KP_MENU  : symbol = POINT; break;
    }
    lcd_print_symbol(lcd, symbol);
}

void kp_print_secure(struct sk_lcd *lcd)
{
    lcd_print_symbol(lcd, POINT);
}
