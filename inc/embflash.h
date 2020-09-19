#include "keypad.h"

/**
 * sk_refresh() - erase all sectors used by keypad.
 *
 * Return: sk_err.
 */
kp_err kp_flash_init();

/**
 * kp_write_settings_to_flash() -  write settings to flash memory.
 * @struct kp_lock *keypad: keypad lock data.
 *
 * Pack keypad settings and add note about them in the firsh empty space
 * in settings_sector in flash memory.
 *
 * Return: kp_err.
 */
kp_err kp_write_settings_to_flash(struct kp_lock *keypad);

/**
 * kp_read_settings_from_flash() -  read settings from flash memory.
 * @struct kp_lock *keypad: keypad lock data.
 *
 * Read recent note about settings from settings_sector in flash memory
 * and unpack it to keypad.
 *
 * Return: kp_err.
 */
kp_err kp_read_settings_from_flash(struct kp_lock *keypad);

/**
 * kp_write_logs_to_flash() -  write log about attemp to unlock keypad to flash memory.
 * @struct kp_lock *keypad: keypad lock data.
 *
 * Pack keypad log and add note about them in the firsh empty space
 * in fail_log_sector in flash memory.
 *
 * Return: kp_err.
 */
kp_err kp_write_logs_to_flash(struct kp_lock *keypad);

/**
 * kp_read_logs_from_flash() -  read settings from flash memory.
 * @struct kp_lock *keypad: keypad lock data.
 *
 * Read recent note about log from fail_log_sector in flash memory
 * and unpack it to keypad.
 *
 * Return: kp_err.
 */
kp_err kp_read_logs_from_flash(struct kp_lock *keypad);

/**
 * kp_if_failed_logs() - check recent fail log.
 *
 * Return: bool.
 * Return true if log about recent failed attempt to unlock keypad metters,
 * else - return false.
 */
bool kp_if_failed_logs(void);

/**
 * kp_if_failed_logs_np() - check recent fail log.
 *
 * Return: bool.
 * Return true if keypad has not react on failed attempt to unlock keypad yet,
 * else - return false.
 */
bool kp_if_failed_logs_np(void);

/**
 * kp_logs_in_flash_successed() - mark recent log as successful attempt.
 *
 * Return: void.
 */
void kp_logs_in_flash_successed(void);

/**
 * kp_logs_in_flash_failed() - mark recent log as failed attempt.
 *
 * Return: void.
 */
void kp_logs_in_flash_failed(void);

/**
 * kp_logs_in_flash_failed() - mark recent log as failed attempt that was 'punished'.
 *
 * Return: void.
 */
void kp_logs_in_flash_failed_p(void);
