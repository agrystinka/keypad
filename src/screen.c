#include "screen.h"
#include "keypad.h" //for STATE_SYMBOL
#include "mygpiolib.h"
#include "timers.h"

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

void kp_screen_welcome(struct sk_lcd *lcd)
{
    sk_lcd_cmd_clear(lcd);
    sk_lcd_cmd_setaddr(lcd, 0x00, false);
    lcd_print_symbol(lcd, UNLOCKED);
    lcd_print(lcd, " Welcome");
}

void kp_screen_fail(struct sk_lcd *lcd)
{
    sk_lcd_cmd_clear(lcd);
    sk_lcd_cmd_setaddr(lcd, 0x00, false);
    lcd_print_symbol(lcd, STATE_SYMBOL);
    lcd_print(lcd, " Access denied");
}
