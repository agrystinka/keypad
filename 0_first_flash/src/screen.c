#include "screen.h"


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
