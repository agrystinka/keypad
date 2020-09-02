#include "screen.h"

void kp_screen_empty(struct sk_lcd *lcd)
{
    for(uint8_t i = 0; i < 2; i++){
        sk_lcd_cmd_setaddr(lcd, i * 0x40, false);
        for(uint8_t j = 0; j < 16; j++)
            sk_lcd_putchar(lcd, ' ');
    }
}

void kp_screen_timer(struct sk_lcd *lcd, uint32_t delay_s)
{
    //deny interrupts
    kp_screen_empty(lcd);

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
    kp_screen_empty(lcd);

    sk_lcd_cmd_setaddr(lcd, 0x00, false);
    lcd_print_symbol(lcd, UNLOCKED);
    lcd_print(lcd, " Welcome");
}

void kp_screen_input(struct sk_lcd *lcd)
{
    kp_screen_empty(lcd);

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
