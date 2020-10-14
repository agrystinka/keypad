#include "keypad.h"
#include "screen.h"
#include "lcd_hd44780.h"

/**
 * kp_btn_enable() - enable interrupts on GLSK SWT buttons.
 *
 * Return: void.
 */
void kp_btn_enable(void);

/**
 * kp_btn_disable() - disable interrupts on GLSK SWT buttons.
 *
 * Return: void.
 */
void kp_btn_disable(void);

/**
 * kp_lock_keypad() - lock keypad.
 * @struct kp_lock *keypad: keypad lock data.
 *
 * Return: void.
 */
void kp_lock_keypad(struct kp_lock *keypad);

/**
 * kp_unlock_keypad() - unlock keypad.
 * @struct kp_lock *keypad: keypad lock data.
 *
 * Return: void.
 */
void kp_unlock_keypad(struct kp_lock *keypad);

/**
 * kp_main_settings() - single openning.
 * @struct sk_lcd *lcd: LCD for comunication with user.
 * @struct kp_lock *keypad: keypad lock data.
 *
 * Unlock keypad for 'delay_open_s' seconds and lock it back. Suited or Mode 1.
 *
 * Return: void.
 */
void kp_single_opening(struct sk_lcd *lcd, struct kp_lock *keypad);

/**
 * kp_main_settings() - change keypad state.
 * @struct sk_lcd *lcd: LCD for comunication with user.
 * @struct kp_lock *keypad: keypad lock data.
 *
 * Unlock keypad if in was locked adn vise versa. Suited or Mode 2.
 *
 * Return: void.
 */
void kp_toggle_keypad_state(struct kp_lock *keypad);

/**
 * kp_welcome() - discard info about fails.
 * @struct kp_lock *keypad: keypad lock data.
 *
 * Return: void.
 */
void kp_discard_fails(struct kp_lock *keypad);


/**
 * kp_welcome() - handle correct User Password input.
 * @struct sk_lcd *lcd: LCD for comunication with user.
 * @struct kp_lock *keypad: keypad lock data.
 *
 * Handle correct User Password input according to keypad mode:
 * unlock keypad for delay_open_s' seconds or switch keypad state.
 *
 * Return: void.
 */
void kp_welcome(struct sk_lcd *lcd, struct kp_lock *keypad);

/**
 * kp_fail() - handle incorrect User Password input.
 * @struct sk_lcd *lcd: LCD for comunication with user.
 * @struct kp_lock *keypad: keypad lock data.
 * @bool count: if true - this fail have to be counted,
 *              if false - jaste show waiting timer
 *
 * Handle correct User Password input.
 * Block keypad for 'delay_wait_cur_s' seconds if there were more or equal to 'fails_low'
 * failed attempts of input User Password.
 * If keypad works in HIGHT_SECURITY mode, handle situations if there were more or equal
 * to 'fails_hight' failed attempts of input User Password.
 *
 * Return: void.
 */
void kp_fail(struct sk_lcd *lcd, struct kp_lock *keypad, bool count);

/**
 * kp_keypad_error() - handle keypad errors.
 * @struct sk_lcd *lcd: LCD for comunication with user.
 *
 * Return: void.
 */
void kp_keypad_error(struct sk_lcd *lcd);
