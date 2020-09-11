#ifndef KP_MENU_H
#define KP_MENU_H

#include "keypad.h"
// #include "menu_template.h"
// #include "display.h"
// #include "errors.h"
// #include "lcd_hd44780.h"
// #include "intrinsics.h"

/**
 * kp_menu() - keypad settings.
 * @struct sk_lcd *lcd: LCD for comunication with user.
 *
 * Return: void.
 */
void kp_menu(struct sk_lcd *lcd);

#endif
