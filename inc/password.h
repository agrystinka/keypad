#ifndef KP_KEYPAD_H
#define KP_KEYPAD_H

#include "lcd_hd44780.h"
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

/**
 * kp_check_plain() - check if passwords are equal.
 * @uint8_t *passw1: original password.
 * @uint8_t *pass2: input password..
 * @uint8_t length: length of paswords.
 *
 * Return: bool; true - if passwords are equal, false - if not.
 */
bool kp_check_plain(uint8_t *password, uint8_t *input, uint8_t len);

/**
 * kp_input_password() - password input handler.
 * @struct sk_lcd *lcd: LCD for comunication with user.
 * @uint8_t *pass: pointer on array store the input password.
 * @uint8_t passlength: lenght of password, that should be input.
 *                      Is used to show user the length of asked pass.
 * @char *instruction: Short message to user (0-16 chars)
 * @bool secure: if true - show only current number, other are masked as *;
 *               if false - show password while user input it.
 *
 * Handle input of password using GLSK SWT buttons.
 *
 * Return: void.
 */
void kp_input_password(struct sk_lcd *lcd, uint8_t *pass, uint8_t passlength, char *instruction, bool secure);

#endif
