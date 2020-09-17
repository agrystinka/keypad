#include "pack.h"

#include <stdio.h>
#include <stdbool.h>

/**
 * pack_fail_log() - pack data about failed attempts to unlock keypad.
 * @uint8_t *buffer: pointer on buffer with size GLOBAL_DATA_SIZE.
 * @struct kp_lock *keypad: keypad lock data.
 *
 * Pack data about keypad to uint8_t buffer of FAIL_LOG_SIZE size.
 *
 * Return: void.
 */
void pack_fail_log(uint8_t *buffer, struct kp_lock *keypad)
{
	buffer[0] = keypad->fails;
	uint32_t *pointer = (uint32_t *) &buffer[1];
	*pointer = keypad->delay_wait_cur_s; //buffer[1] - buffer[4]
}

/**
 * unpack_fail_log() - unpack data about failed attempts to unlock keypad.
 * @uint8_t *buffer: pointer on buffer with size GLOBAL_DATA_SIZE.
 * @struct kp_lock *keypad: keypad lock data.
 *
 * Unpack data about keypad from uint8_t buffer of FAIL_LOG_SIZE size.
 *
 * Return: void.
 */
void unpack_fail_log(uint8_t *buffer, struct kp_lock *keypad)
{
	keypad->fails = buffer[0];
	uint32_t *pointer = (uint32_t *) &buffer[1];
	keypad->delay_wait_cur_s = *pointer;
}

/**
 * pack_settings() - pack keypad settings.
 * @uint8_t *buffer: pointer on buffer with size GLOBAL_DATA_SIZE.
 * @struct kp_lock *keypad: keypad lock data.
 *
 * Pack data about keypad to uint8_t buffer of SETTINGS_SIZE size.
 *
 * Return: void.
 */
void pack_settings(uint8_t *buffer, struct kp_lock *keypad)
{
	int32_t shift = 0;
	//pack User password and Master password
	for(int i = 0; i < MAX_PASS_LENGTH; i++)
		buffer[shift + i] = keypad->usrpass[i];
	shift += MAX_PASS_LENGTH;

	for(int i = 0; i < MAX_PASS_LENGTH; i++)
		buffer[shift + i] = keypad->mstrpass[i];
	shift += MAX_PASS_LENGTH;

	//pack defauld Menucode that could not be changed by user from menu
	for(int i = 0; i < MAX_PASS_LENGTH; i++)
		buffer[shift + i] = keypad->menucode[i];
	shift += MAX_PASS_LENGTH;

	//pack pass length: 2 * 1 byte = 2 bytes
	buffer[shift + 0] = keypad->usrpass_length;
	buffer[shift + 1] = keypad->mstrpass_length;
	shift += 2;

	//pack delays  (uint32_t variables) 3 * 4 bytes= 12 bytes
	uint32_t *pointer32 = (uint32_t *)&buffer[shift];
	*(pointer32 + 0) = keypad->delay_open_s;
	*(pointer32 + 1) = keypad->delay_wait_s;
	*(pointer32 + 2) = keypad->delay_wait_cur_s;
	shift += 12;

	//pack other settings: 3 bytes
	buffer[shift + 0] = keypad->fails;
	buffer[shift + 1] = keypad->fails_low;
	buffer[shift + 2] = keypad->wait_coef;
	shift += 3;

	//pack bool variables: 2 bytes
	if(keypad->mode)
		buffer[shift + 0] = 1;
	else
		buffer[shift + 0] = 0;
	//state is always saved as locked(closed) in order to provide more security
	buffer[shift + 1] = 0;
	shift += 2;
	//total: 3 * MAX_PASS_LENGTH + (2 + 12 + 3 + 2) = 3 * MAX_PASS_LENGTH + 19
#if  HIGHT_SECURITY
	//pack Semimaster password
	for(int i = 0; i < MAX_PASS_LENGTH; i++)
		buffer[shift + i] = keypad->semimstrpass[i];
	shift += MAX_PASS_LENGTH;
	//pack Semimaster password length: 1 byte
	buffer[shift + 0] = keypad->mstrpass_length;
	//pack other settings: 1 byte
	buffer[shift + 1] = keypad->fails_high;
	shift += 2;
	//total: main settings + extended settings =
	// (3 * MAX_PASS_LENGTH + 19) + (MAX_PASS_LENGTH + 1 + 1) =
	// 4 * MAX_PASS_LENGTH + 21
#endif
}

/**
 * unpack_settings() - unpack keypad settings.
 * @uint8_t *buffer: pointer on buffer with size GLOBAL_DATA_SIZE.
 * @struct kp_lock *keypad: keypad lock data.
 *
 * Unpack data about keypad from uint8_t buffer of SETTINGS_SIZE size.
 *
 * Return: void.
 */
void unpack_settings(uint8_t *buffer, struct kp_lock *keypad)
 {
 	int32_t shift = 0;
 	//unpack User password and Master password
 	for(int i = 0; i < MAX_PASS_LENGTH; i++)
 		keypad->usrpass[i] = buffer[shift + i];
 	shift += MAX_PASS_LENGTH;

 	for(int i = 0; i < MAX_PASS_LENGTH; i++)
 		keypad->mstrpass[i] = buffer[shift + i];
 	shift += MAX_PASS_LENGTH;

 	//unpack defauld Menucode that could not be changed by user from menu
 	for(int i = 0; i < MAX_PASS_LENGTH; i++)
 		 keypad->menucode[i] = buffer[shift + i];
 	shift += MAX_PASS_LENGTH;

 	//pack pass length: 2 * 1 byte = 2 bytes
 	keypad->usrpass_length = buffer[shift + 0];
 	keypad->mstrpass_length = buffer[shift + 1];
 	shift += 2;

 	//unpack delays  (uint32_t variables) 3 * 4 bytes= 12 bytes
 	uint32_t *pointer32 = (uint32_t *)&buffer[shift];
 	keypad->delay_open_s = *(pointer32 + 0);
 	keypad->delay_wait_s = *(pointer32 + 1);
 	keypad->delay_wait_cur_s = *(pointer32 + 2);
 	shift += 12;

 	//unpack other settings: 3 bytes
 	keypad->fails = buffer[shift + 0];
 	keypad->fails_low = buffer[shift + 1];
 	keypad->wait_coef = buffer[shift + 2];
 	shift += 3;

 	//unpack bool variables: 2 bytes
 	if(buffer[shift + 0])
 		keypad->mode = true;
 	else
 		keypad->mode = false;
 	//state is always saved as locked(closed) in order to provide more security
 	keypad->state = false;
 	shift += 2;
 	//total: 3 * MAX_PASS_LENGTH + (2 + 12 + 3 + 2) = 3 * MAX_PASS_LENGTH + 19
 #if  HIGHT_SECURITY
 	//unpack Semimaster password
 	for(int i = 0; i < MAX_PASS_LENGTH; i++)
 		keypad->semimstrpass[i] = buffer[shift + i];
 	shift += MAX_PASS_LENGTH;
 	//unpack Semimaster password length: 1 byte
 	keypad->mstrpass_length = buffer[shift + 0];
 	//unpack other settings: 1 byte
 	keypad->fails_high = buffer[shift + 1];
 	shift += 2;
 	//total: main settings + extended settings =
 	// (3 * MAX_PASS_LENGTH + 19) + (MAX_PASS_LENGTH + 1 + 1) =
 	// 4 * MAX_PASS_LENGTH + 21
 #endif
 }
