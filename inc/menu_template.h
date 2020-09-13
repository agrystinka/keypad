#ifndef KP_MENU_TEMPLATE_H
#define KP_MENU_TEMPLATE_H

#include "keypad.h"  //for MAX_PASS_LENGTH definition
#include "lcd_hd44780.h"
#include <stdint.h>
#include <stdio.h>

#define GO_BACK 0

/** Present custom menu data as following structure to use menu_template()*/
struct __attribute__((packed, aligned(1))) menu{
    /**Amount of options*/
    uint8_t num;
    /** Names of options (up to 16 chars)*/
    char** lines;
    /** Pointers on fuinctions that realize every options. First option should be GO_BACK*/
    void (**options)(struct sk_lcd *lcd, struct kp_lock *keypad);
};

/**
 * kp_menu_template() - template of menu for keypad.
 * @struct sk_lcd *lcd: LCD for comunication with user.
 * @struct kp_lock *keypad: keypad lock data.
 * @struct menu *menu: the information about menu, you want to create.
 *
 * Function hadle keypad menu using GLSK UP, DOWN, MENU buttons.
 * To create own menu, firstly initialize struct menu *menu with:
 * lines(options) you need and functions which are realizing this options.
 * To get more info read about struct menu.
 *
 * Return: void.
 */
void kp_menu_template(struct sk_lcd *lcd, struct kp_lock *keypad, struct menu *menu);

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
