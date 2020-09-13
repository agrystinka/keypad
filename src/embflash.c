#include "keypad.h"

#define GLOBAL_DATA_SIZE 60
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
	uint32_t *pointer32 = (uint32_t *)buffer;
	//pack delays  (uint32_t variables) 4 * 3 = 12 bytes
	uint32_t shift = 12;
	*(pointer32 + 0) = keypad->delay_open_s;
	*(pointer32 + 1) = keypad->delay_wait_s;
	*(pointer32 + 2) = keypad->delay_wait_cur_s;
	//pack uint8_t variables
	//pack passwords: 2 * 8  = 16 bytes(if MAX_PASS_LENGTH == 8)
	for(int i = 0; i < MAX_PASS_LENGTH; i++)
		buffer[shift + i] = keypad->usrpass[i];
	shift += MAX_PASS_LENGTH;
	for(int i = 0; i < MAX_PASS_LENGTH; i++)
		buffer[shift + i] = keypad->mstrpass[i];
	shift += MAX_PASS_LENGTH;
	//pack pass kp_set_length: 2 bytes
	buffer[shift + 0] = keypad->usrpass_length;
	buffer[shift + 1] = keypad->mstrpass_length;
	//pack settings: 5 bytes
	buffer[shift + 2] = keypad->fails;
	buffer[shift + 3] = keypad->fails_low;
	buffer[shift + 4] = keypad->fails_high;
	buffer[shift + 5] = keypad->wait_coef;
	if(keypad->mode)
		buffer[shift + 6] = 1;
	else
		buffer[shift + 6] = 0;
	//total: 12 + 16 + 2 + 5 = 35 byte = 9 words
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
	uint32_t *pointer32 = (uint32_t *)buffer;
	//pack delays  (uint32_t variables) 4 * 3 = 12 bytes
	uint32_t shift = 12;
	keypad->delay_open_s = *(pointer32 + 0);
	keypad->delay_wait_s = *(pointer32 + 1);
	keypad->delay_wait_cur_s = *(pointer32 + 2);
	//pack uint8_t variables
	//pack passwords: 2 * 8  = 16 bytes(if MAX_PASS_LENGTH == 8)
	uint8_t *pointer8 = (uint8_t *) &buffer[3];
	for(int i = 0; i < MAX_PASS_LENGTH; i++)
		keypad->usrpass[i] = buffer[shift + i];
	shift += MAX_PASS_LENGTH;
	for(int i = 0; i < MAX_PASS_LENGTH; i++)
		keypad->mstrpass[i] = buffer[shift + i];
	shift += MAX_PASS_LENGTH;
	//pack pass kp_set_length: 2 bytes
	keypad->usrpass_length = buffer[shift + 0];
	keypad->mstrpass_length = buffer[shift + 1];
	//pack settings: 5 bytes
	keypad->fails = buffer[shift + 3];
	keypad->fails_low = buffer[shift + 3];
	keypad->fails_high =buffer[shift + 4];
	keypad->wait_coef = buffer[shift + 5];
	if(buffer[shift + 6])
		keypad->mode = true;
	else
		keypad->mode = false;
	//total: 12 + 16 + 2 + 5 = 35 byte = 9 words
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
	//block buttons just in case
	kp_btn_disable();
	//switch ON signal that settings are being written to flash memory
	mgl_set(mgl_led_orange);

	uint8_t data[GLOBAL_DATA_SIZE];
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

#if SEMIHOSTING_USE
	printf("\n-------------------Write DATA PACKED\n");
	for(uint8_t i = 0; i < 35; i++)
		printf("%x", data[i]);
#endif
	//switch OFF signal that settings are being written to flash memory
	mgl_clear(mgl_led_orange);
	//unblock buttons
	kp_btn_enable();
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
	uint8_t data[GLOBAL_DATA_SIZE];
    uint8_t *addr;
    addr = ADRESS_FLASH;
    for(uint32_t i = 0; i < GLOBAL_DATA_SIZE; i++){
        data[i] = *addr;
        addr += sizeof(data[i]);
		delay_ms_systick(1);
    }

#if SEMIHOSTING_USE
	printf("\n-------------------Read DATA PACKED\n");
	for(uint8_t i = 0; i < 35; i++)
		printf("%x", data[i]);
#endif
	//Memory Barrier
	__DMB();
	unpack_keypad_data(&data[0], keypad);
}
