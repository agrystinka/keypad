#include "keypad.h"

/**
 * kp_fail() - handle incorrect User Password input.
 * @struct sk_lcd *lcd: LCD for comunication with user.
 * @struct kp_lock *keypad: keypad lock data.
 *
 * Handle correct User Password input.
 * Block keypad for 'delay_wait_cur_s' seconds if there were more or equal to 'fails_low'
 * failed attempts of input User Password.
 * If keypad works in HIGHT_SECURITY mode, handle situations if there were more or equal
 * to 'fails_hight' failed attempts of input User Password.
 *
 * Return: void.
 */

//void write_keypad_data_to_flash(struct kp_lock *keypad);

/**
 * kp_fail() - handle incorrect User Password input.
 * @struct sk_lcd *lcd: LCD for comunication with user.
 * @struct kp_lock *keypad: keypad lock data.
 *
 * Handle correct User Password input.
 * Block keypad for 'delay_wait_cur_s' seconds if there were more or equal to 'fails_low'
 * failed attempts of input User Password.
 * If keypad works in HIGHT_SECURITY mode, handle situations if there were more or equal
 * to 'fails_hight' failed attempts of input User Password.
 *
 * Return: void.
 */

//void read_keypad_data_from_flash(struct kp_lock *keypad);

sk_err kp_flash_init();

void kp_write_settings_to_flash(struct kp_lock *keypad);
void kp_read_settings_from_flash(struct kp_lock *keypad);
