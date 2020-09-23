#include "keypad.h"
#include <stdint.h>

#if  HIGHT_SECURITY
#define SETTINGS_SIZE (4 * MAX_PASS_LENGTH + 21)
#else
#define SETTINGS_SIZE (3 * MAX_PASS_LENGTH + 19)
#endif

//1 byte (fails) + 4 bytes (delay_wait_cur_s) = 5 bytes
#define FAIL_LOG_SIZE 5

/**
 * pack_fail_log() - pack data about failed attempts to unlock keypad.
 * @uint8_t *buffer: pointer on buffer with size GLOBAL_DATA_SIZE.
 * @struct kp_lock *keypad: keypad lock data.
 *
 * Pack data about keypad to uint8_t buffer of FAIL_LOG_SIZE size.
 *
 * Return: void.
 */
void pack_fail_log(uint8_t *buffer, struct kp_lock *keypad);

/**
 * unpack_fail_log() - unpack data about failed attempts to unlock keypad.
 * @uint8_t *buffer: pointer on buffer with size GLOBAL_DATA_SIZE.
 * @struct kp_lock *keypad: keypad lock data.
 *
 * Unpack data about keypad from uint8_t buffer of FAIL_LOG_SIZE size.
 *
 * Return: void.
 */
void unpack_fail_log(uint8_t *buffer, struct kp_lock *keypad);

/**
 * pack_settings() - pack keypad settings.
 * @uint8_t *buffer: pointer on buffer with size GLOBAL_DATA_SIZE.
 * @struct kp_lock *keypad: keypad lock data.
 *
 * Pack data about keypad to uint8_t buffer of SETTINGS_SIZE size.
 *
 * Return: void.
 */
void pack_settings(uint8_t *buffer, struct kp_lock *keypad);

/**
 * unpack_settings() - unpack keypad settings.
 * @uint8_t *buffer: pointer on buffer with size GLOBAL_DATA_SIZE.
 * @struct kp_lock *keypad: keypad lock data.
 *
 * Unpack data about keypad from uint8_t buffer of SETTINGS_SIZE size.
 *
 * Return: void.
 */
void unpack_settings(uint8_t *buffer, struct kp_lock *keypad);
