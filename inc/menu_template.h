#ifndef KP_MENU_TEMPLATE_H
#define KP_MENU_TEMPLATE_H

#include "keypad.h"  //for MAX_PASS_LENGTH definition
#include "display.h"
#include "lcd_hd44780.h"
#include <stdint.h>
#include <stdio.h>

#define GO_BACK 0

struct menu {
    uint8_t num;
    char** lines;
    void (**options)(struct sk_lcd *lcd);
};

/**
 * kp_menu_template() - template of menu for keypad.
 * @struct sk_lcd *lcd: LCD for comunication with user.
 * @struct menu *menu: the information about menu, you want to create.
 *
 * Function hadle keypad menu using GLSK UP, DOWN, MENU buttons.
 * To create own menu, firstly initialize struct menu *menu with:
 * lines(options) you need and functions which are realizing this options.
 * To get more info read about struct menu.
 *
 * Return: void.
 */
void kp_menu_template(struct sk_lcd *lcd, struct menu *menu);

/**
 * kp_menu_template() - template of menu for keypad.
 * @struct sk_lcd *lcd: LCD for comunication with user.
 * @uint8_t *num: pointer on variable that need to be set.
 *
 * Function hadle GLSK buttons UP, DOWN, RIGHT, LEFT, MENU
 * to make user chose the value from 1 to 60.
 * Then this function set chosen value to variable pointed by num.
 *
 * Return: void.
 */
void set_1_60(struct sk_lcd *lcd, uint8_t *num);

/**
 * kp_menu_template() - template of menu for keypad.
 * @struct sk_lcd *lcd: LCD for comunication with user.
 * @uint8_t *num: pointer on variable that need to be set.
 * @uint8_t *options: options of *num value.
 * @uint8_t size: amount of options (size of array).
 *
 * Function hadle GLSK buttons UP, DOWN, MENU
 * to make user chose the value from given options.
 * Then this function set chosen value to variable pointed by num.
 *
 * Return: void.
 */
void kp_scroll_num(struct sk_lcd *lcd, uint8_t *num, uint8_t *options, uint8_t size);
#endif
