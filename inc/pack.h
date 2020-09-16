#include "keypad.h"
#include <stdint.h>

#if  HIGHT_SECURITY
#define SETTINGS_SIZE (4 * MAX_PASS_LENGTH + 21)
#else
#define SETTINGS_SIZE (3 * MAX_PASS_LENGTH + 19)
#endif

//1 byte (fails) + 4 bytes (delay_wait_cur_s) = 5 bytes
#define FAIL_LOG_SIZE 5

void pack_fail_log(uint8_t *buffer, struct kp_lock *keypad);
void unpack_fail_log(uint8_t *buffer, struct kp_lock *keypad)

void pack_settings(uint8_t *buffer, struct kp_lock *keypad);
void unpack_settings(uint8_t *buffer, struct kp_lock *keypad);
