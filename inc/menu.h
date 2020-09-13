#ifndef KP_MENU_H
#define KP_MENU_H

#include "keypad.h"

/**
 * kp_menu() - keypad settings.
 * @struct sk_lcd *lcd: LCD for comunication with user.
 * @struct kp_lock *keypad: keypad lock data.
 *
 * Return: void.
 */
void kp_menu(struct sk_lcd *lcd, struct kp_lock *keypad);

#endif
