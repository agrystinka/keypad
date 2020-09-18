#include "keypad.h"

kp_err kp_flash_init();
kp_err kp_write_settings_to_flash(struct kp_lock *keypad);
kp_err kp_read_settings_from_flash(struct kp_lock *keypad);
kp_err kp_write_logs_to_flash(struct kp_lock *keypad);
kp_err kp_read_logs_from_flash(struct kp_lock *keypad);


/**
 * kp_if_failed_logs() - check recent fail log.
 * Return: bool.
 * Return true if log about recent failed attempt to unlock keypad metters,
 * else - return false.
 */
bool kp_if_failed_logs(void);

/**
 * kp_if_failed_logs_np() - check recent fail log.
 * Return: bool.
 * Return true if keypad has not react on failed attempt to unlock keypad yet,
 * else - return false.
 */
bool kp_if_failed_logs_np(void);
void kp_logs_in_flash_successed(void);
void kp_logs_in_flash_failed(void);
void kp_logs_in_flash_failed_p(void);
