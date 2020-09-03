#include "screen.h"

void kp_screen_empty(struct sk_lcd *lcd)
{
    sk_lcd_cmd_clear(lcd);
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

void kp_screen_input(struct sk_lcd *lcd)
{
    sk_lcd_cmd_clear(lcd);

    sk_lcd_cmd_setaddr(lcd, 0x00, false);
    lcd_print_symbol(lcd, LOCKED);
    lcd_print(lcd, " Password:");

    sk_lcd_cmd_setaddr(lcd, 0x40, false);
    lcd_print(lcd, "\t");

    for(uint8_t i = 0;i < PASS_LENGTH; i++)
        lcd_print(lcd, "_");

    sk_lcd_cmd_setaddr(lcd, 0x40, false);
    lcd_print(lcd, "\t");
}

void kp_screen_menu(struct sk_lcd *lcd)
{
    sk_lcd_cmd_clear(lcd);

    sk_lcd_cmd_setaddr(lcd, 0x00, false);
    lcd_print_symbol(lcd, LOCKED);
    lcd_print(lcd, " Menu");
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

// #include "screen.h"
//
// // void kp_screen_empty(struct sk_lcd *lcd)
// // {
// //     for(uint8_t i = 0; i < 2; i++){
// //         sk_lcd_cmd_setaddr(lcd, i * 0x40, false);
// //         for(uint8_t j = 0; j < 16; j++)
// //             sk_lcd_putchar(lcd, ' ');
// //     }
// // }
//
// void kp_screen_timer(struct sk_lcd *lcd, uint32_t delay_s)
// {
//     //deny interrupts
//     sk_lcd_cmd_clear(lcd);
//
//     sk_lcd_cmd_setaddr(lcd, 0x00, false);
//     lcd_print_symbol(lcd, LOCKED);
//     lcd_print(lcd, " Access denied");
//
//     for(int i = delay_s; i >= 0; i--){
//         lcd_print_time(lcd, i);
//         delay_ms_systick(1000);
//     }
// }
//
// void kp_screen_welcome(struct sk_lcd *lcd)
// {
//     sk_lcd_cmd_clear(lcd);
//
//     sk_lcd_cmd_setaddr(lcd, 0x00, false);
//     lcd_print_symbol(lcd, UNLOCKED);
//     lcd_print(lcd, " Welcome");
//
//     //sk_lcd_cmd_setaddr(&lcd, 0x10, false); //move blinking cursor out
// }
//
// void kp_screen_input(struct sk_lcd *lcd)
// {
//     sk_lcd_cmd_clear(lcd);
//
//     sk_lcd_cmd_setaddr(lcd, 0x00, false);
//     lcd_print_symbol(lcd, UNLOCKED);
//     lcd_print(lcd, " Password: ");
//
//     sk_lcd_cmd_setaddr(lcd, 0x40, false);
// 	lcd_print(lcd, "\t");
//
//     for(uint8_t i = 0;i < PASS_LENGTH; i++)
//         lcd_print(lcd, "_");
//
//     sk_lcd_cmd_setaddr(lcd, 0x40, false);
//     lcd_print(lcd, "\t");
// }
