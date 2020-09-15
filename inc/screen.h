#ifndef KP_SCREEN_H
#define KP_SCREEN_H

#include "display.h"
#include "errors.h"
#include "lcd_hd44780.h"
#include <stdint.h>
#include <stdio.h>

#define PASS_SHIFT 2

/**
 * kp_screen_empty() - clear screen.
 * @struct sk_lcd *lcd: LCD for comunication with user.
 *
 * An abstraction over lcd_hd44780
 * Return: void.
 */
void kp_screen_empty(struct sk_lcd *lcd);

/**
 * kp_screen_message() - print message on screen.
 * @struct sk_lcd *lcd: LCD for comunication with user.
 * @char *part1: first part of mesage(first line) - up to 16 chars.
 * @char *part2: second part of mesage(second line) - up to 16 chars.
 *
 * Return: void.
 */
void kp_screen_message(struct sk_lcd *lcd, char *part1, char *part2);

/**
 * kp_screen_message() - print field for pass input.
 * @struct sk_lcd *lcd: LCD for comunication with user.
 * @uint8_t passlength: length of input.
 * @char *instruction: message to user (name of password, etc.)
 *
 * Return: void.
 */
void kp_screen_input(struct sk_lcd *lcd, uint8_t passlength, char *instruction);

/**
 * kp_screen_message() - make delay and print the timer state on display.
 * @struct sk_lcd *lcd: LCD for comunication with user.
 * @uint32_t delay_s: delay in seconds.
 * @uint8_t line: line in which timer state need to be printed
 *                if 0 - first line, else - second line
 *
 * Return: void.
 */
void kp_screen_timer(struct sk_lcd *lcd, uint32_t delay_s, uint8_t line);

/**
 * kp_screen_message() - show WELCOME screen.
 * @struct sk_lcd *lcd: LCD for comunication with user.
 *
 * Custom function suited for keypad lock. Show keypad lock STATE (LOCED or UNLOCKED)
 * Print geeting to user.
 *
 * Return: void.
 */
void kp_screen_welcome(struct sk_lcd *lcd);

/**
 * kp_screen_message() - show ACCESS DENIED screen.
 * @struct sk_lcd *lcd: LCD for comunication with user.
 *
 * Custom function suited for keypad lock. Show keypad lock STATE (LOCED or UNLOCKED)
 * Print message to user.
 *
 * Return: void.
 */
void kp_screen_fail(struct sk_lcd *lcd);

#endif
