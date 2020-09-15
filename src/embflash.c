#include "embflash.h"
#include <stdio.h>

//#if  HIGHT_SECURITY
// #define GLOBAL_DATA_SIZE (4 * MAX_PASS_LENGTH + 21)
// #else
// #define GLOBAL_DATA_SIZE (3 * MAX_PASS_LENGTH + 19)
// #endif
#define GLOBAL_DATA_SIZE 100

#define ADRESS_FLASH 0x080E0000
#define SECTOR_FLASH 11

/**
 * pack_keypad_data() - pack data about keypad.
 * @uint8_t *buffer: pointer on buffer with size GLOBAL_DATA_SIZE.
 * @struct kp_lock *keypad: keypad lock data.
 *
 * Pack data about keypad that might be set by user to uint8_t buffer
 * whith GLOBAL_DATA_SIZE elements
 *
 * Return: void.
 */
static void pack_keypad_data(uint8_t *buffer, struct kp_lock *keypad)
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
 * unpack_keypad_data() - unpack data about keypad.
 * @uint8_t *buffer: pointer on buffer with size GLOBAL_DATA_SIZE.
 * @struct kp_lock *keypad: keypad lock data.
 *
 * Unpack data about keypad from buffer to kp_lock structure.
 *
 * Return: void.
 */
 static void unpack_keypad_data(uint8_t *buffer, struct kp_lock *keypad)
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

/**
 * write_keypad_data_to_flash() - write keypad data to non-volitile embedded flash memory.
 * @struct kp_lock *keypad: keypad lock data.
 *
 * This operation ned some time (up to 2 seconds). While it heppends the orange LED is on.
 * Return: void.
 */
void write_keypad_data_to_flash(struct kp_lock *keypad)
{
#if !FIRST_FLASH
	kp_btn_disable(); //block buttons just in case
#endif

	//switch ON signal that settings are being written to flash memory
	mgl_set(mgl_led_orange);

	volatile uint8_t data[GLOBAL_DATA_SIZE];
    //unlock flash memory to be able write to it
	flash_unlock();
	//erase sector befor writing to it
    flash_erase_sector(11, 0);//sector - 11, program size - 8bit (0)
	//Pack global variables to array
	pack_keypad_data(&data[0], keypad);
	//Memory Barrier
	__DMB();
    //Check that no main Flash memory operation is ongoing
    //set PG to FLASH_CR, perform the data write operation
	flash_program(ADRESS_FLASH, &data[0], GLOBAL_DATA_SIZE);
	//lock flash memoty to prevent spurious writes
	flash_lock();

// #if SEMIHOSTING_USE
// 	printf("\n-------------------Write DATA PACKED\n");
// 	for(uint8_t i = 0; i < GLOBAL_DATA_SIZE; i++)
// 		printf("%x", data[i]);
// #endif

	//switch OFF signal that settings are being written to flash memory
	mgl_clear(mgl_led_orange);

#if !FIRST_FLASH
	kp_btn_enable(); //unblock buttons
#endif
}

/**
 * read_keypad_data_from_flash() - read keypad data from non-volitile embedded flash memory.
 * @struct kp_lock *keypad: keypad lock data.
 *
 * According to keypad settings, Core works on 16MHz frequency with 3.3 voltage.
 * That means that dtat could be read from flash without waiting cycles.
 *
 * Return: void.
 */
void read_keypad_data_from_flash(struct kp_lock *keypad)
{
#if !FIRST_FLASH
	kp_btn_disable(); //block buttons just in case
#endif

	volatile uint8_t data[GLOBAL_DATA_SIZE];
    uint8_t *addr;
    addr = ADRESS_FLASH;
    for(uint32_t i = 0; i < GLOBAL_DATA_SIZE; i++){
        data[i] = *addr;
        addr += sizeof(data[i]);
	//	delay_ms_systick(1);
    }

// #if SEMIHOSTING_USE
// 	printf("\n-------------------Read DATA PACKED\n");
// 	for(uint8_t i = 0; i < GLOBAL_DATA_SIZE; i++)
// 		printf("%x", data[i]);
// #endif

	//Memory Barrier
	__DMB();
	unpack_keypad_data(&data[0], keypad);

#if !FIRST_FLASH
	kp_btn_enable(); //unblock buttons
#endif
}
