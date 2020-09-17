#include "embflash.h"
#include "pack.h"
#include "embedded_flash.h"
#include <stdio.h>

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

sk_err kp_flash_init()
{
	flash_unlock();
	//erase sectors before writing to it
    flash_erase_sector(settings_sector.num, 0);//sector - 11, program size - 8bit (0)
	//flash_erase_sector(SECTOR_NUM_10, 0);//sector - 11, program size - 8bit (0)
//	flash_erase_sector(SECTOR_NUM_9, 0);//sector - 11, program size - 8bit (0)
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

	flash_unlock();
	flash_erase_sector(SECTOR_NUM_11, 0);
	volatile uint8_t data[SETTINGS_SIZE];
	//Pack keypad settings to array
	pack_settings(&data[0], keypad);

	__DMB();

	flash_program(SECTOR_ADDRESS_11, &data[0], SETTINGS_SIZE);

	flash_lock();

#if SEMIHOSTING_USE
	printf("Write DATA\n");
	for(uint32_t i = 0; i < SETTINGS_SIZE; i++)
		printf("%x ", data[i]);
	printf("\n");
#endif

#if !FIRST_FLASH
	kp_btn_enable(); //unblock buttons
#endif
}


void kp_read_settings_from_flash(struct kp_lock *keypad)
{
#if !FIRST_FLASH
	kp_btn_disable(); //block buttons just in case
#endif

	volatile uint8_t data[SETTINGS_SIZE];
	//sk_flash_read(&data[0], SETTINGS_SIZE, SECTOR_ADDRESS_11);
	for(uint32_t i = 0; i < SETTINGS_SIZE; i++)
		data[i] = *((uint8_t *) (SECTOR_ADDRESS_11 + i));
	__DMB();

#if SEMIHOSTING_USE
	printf("Read DATA\n");
	for(uint32_t i = 0; i < SETTINGS_SIZE; i++)
		printf("%x ", data[i]);
	printf("\n");
#endif
	unpack_settings(&data[0], keypad);

#if !FIRST_FLASH
	kp_btn_enable(); //unblock buttons
#endif
}
