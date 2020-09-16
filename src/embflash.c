#include "embflash.h"
//#include "pack.h"
#include "embedded_flash.h"


#if  HIGHT_SECURITY
#define SETTINGS_SIZE (4 * MAX_PASS_LENGTH + 21)
#else
#define SETTINGS_SIZE (3 * MAX_PASS_LENGTH + 19)
#endif

//1 byte (fails) + 4 bytes (delay_wait_cur_s) = 5 bytes
#define FAIL_LOG_SIZE 5

//initializations of sectors you need
struct sk_sector settings_sector = {
	.start = SECTOR_ADDRESS_11,
	.size = SECTOR_128_SIZE,
	.num = SECTOR_NUM_11,
};

struct sk_sector fail_log_sector = {
	.start = SECTOR_ADDRESS_10,
	.size = SECTOR_128_SIZE,
	.num = SECTOR_NUM_10,
};

struct sk_sector reserved_sector = {
	.start = SECTOR_ADDRESS_9,
	.size = SECTOR_128_SIZE,
	.num = SECTOR_NUM_9,
};


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
	*pointer = keypad->delay_wait_cur_s;
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

	volatile uint8_t data[SETTINGS_SIZE];
    //unlock flash memory to be able write to it
	flash_unlock();
	//erase sector befor writing to it
    flash_erase_sector(11, 0);//sector - 11, program size - 8bit (0)
	//Pack global variables to array
	pack_settings(&data[0], keypad);
	//Memory Barrier
	__DMB();
    //Check that no main Flash memory operation is ongoing
    //set PG to FLASH_CR, perform the data write operation
	flash_program(SECTOR_ADDRESS_11, &data[0], SETTINGS_SIZE);
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

	volatile uint8_t data[SETTINGS_SIZE];
    uint8_t *addr;
    addr = SECTOR_ADDRESS_11;
    for(uint32_t i = 0; i < SETTINGS_SIZE; i++){
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
	unpack_settings(&data[0], keypad);

#if !FIRST_FLASH
	kp_btn_enable(); //unblock buttons
#endif

}

sk_err kp_flash_init()
{
	flash_unlock();
	//erase sectors before writing to it
    flash_erase_sector(SECTOR_NUM_11, 0);//sector - 11, program size - 8bit (0)
	flash_erase_sector(SECTOR_NUM_10, 0);//sector - 11, program size - 8bit (0)
	flash_erase_sector(SECTOR_NUM_9, 0);//sector - 11, program size - 8bit (0)
	//lock flash memoty to prevent spurious writes
	flash_lock();
	return SK_EOK;
}

//---------------------new read write

void kp_write_settings_to_flash(struct kp_lock *keypad)
{
#if !FIRST_FLASH
	kp_btn_disable(); //block buttons just in case
#endif

	//switch ON signal that settings are being written to flash memory
	mgl_set(mgl_led_orange);

	volatile uint8_t data[SETTINGS_SIZE];
	//Pack keypad settings to array
	pack_settings(&data[0], keypad);
	//find space in flash memory
	data_t *address = sk_find_space(&settings_sector, SETTINGS_SIZE);
	//Memory Barrier
	__DMB();

	if(address == NULL){
		//save latest data into reserved_sector
		//tag reserved sector as settings_sector
		//erase settings_sector
		//tag settings_sector as reserved_sector
		//write settings by address
		//calculete new address in settings_sector
	}
	//write settings by address

    //unlock flash memory to be able write to it
	flash_unlock();
    //Check that no main Flash memory operation is ongoing
    //set PG to FLASH_CR, perform the data write operation
	flash_program(SECTOR_ADDRESS_11, &data[0], SETTINGS_SIZE);
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
