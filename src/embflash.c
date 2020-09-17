#include "embflash.h"
#include "pack.h"
#include "embedded_flash.h"
#include <stdio.h>

//1111 1110
#define ATTEMPT_CODE 0xfe
//1111 1100
#define SUCCESS_CODE 0xfc

//initializations of sectors you need
struct sk_sector settings_sector = {
	.start = SECTOR_ADDRESS_11,
	.size = SECTOR_128_SIZE,
	.num = SECTOR_NUM_11,
};

struct sk_sector fail_log_sector = {
	.start = SECTOR_ADDRESS_11,
	.size = SECTOR_128_SIZE,
	.num = SECTOR_NUM_11,
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
//    flash_erase_sector(settings_sector.num, 0);//sector - 11, program size - 8bit (0)
	flash_erase_sector(fail_log_sector.num, 0);//sector - 10, program size - 8bit (0)
//	flash_erase_sector(SECTOR_NUM_9, 0);//sector - 11, program size - 8bit (0)
	//lock flash memoty to prevent spurious writes
	flash_lock();
	return SK_EOK;
}


void kp_write_settings_to_flash(struct kp_lock *keypad)
{
#if !FIRST_FLASH
	kp_btn_disable(); //block buttons just in case
#endif

	volatile uint8_t data[SETTINGS_SIZE];
	//Pack keypad settings to array
	pack_settings(&data[0], keypad);
	uint32_t address = sk_search(&settings_sector, SETTINGS_SIZE, true);
	if(address == NULL);

	__DMB(); //Memory barrier
	sk_flash_write(&data[0], SETTINGS_SIZE, address);

#if SEMIHOSTING_USE
	printf("Write DATA\n");
	for(uint32_t i = 0; i < SETTINGS_SIZE; i++)
		printf("%x ", data[i]);
	printf("\n");
	printf("Write DATA ADDRESS %x\n", address);
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

	uint32_t address = sk_search(&settings_sector, SETTINGS_SIZE, false);
	if(address == NULL);

	volatile uint8_t data[SETTINGS_SIZE];
	sk_flash_read(&data[0], SETTINGS_SIZE, address);

	__DMB(); //Memory barrier
	unpack_settings(&data[0], keypad);

#if SEMIHOSTING_USE
	printf("Read DATA\n");
	for(uint32_t i = 0; i < SETTINGS_SIZE; i++)
		printf("%x ", data[i]);
	printf("\n");
	printf("Read DATA ADDRESS %x\n", address);
#endif
#if !FIRST_FLASH
	kp_btn_enable(); //unblock buttons
#endif
}

void kp_write_logs_to_flash(struct kp_lock *keypad)
{
#if !FIRST_FLASH
	kp_btn_disable(); //block buttons just in case
#endif

	volatile uint8_t data[FAIL_LOG_SIZE + 2];
	pack_fail_log(&data[0], keypad);
	data[FAIL_LOG_SIZE + 0] = 0xff;
	data[FAIL_LOG_SIZE + 1] = 0x00;
	uint32_t address = sk_search(&fail_log_sector, FAIL_LOG_SIZE + 2, true);
	if(address == NULL);

	__DMB(); //Memory barrier
	sk_flash_write(&data[0], FAIL_LOG_SIZE + 2, address);

#if SEMIHOSTING_USE
	printf("Write FAIL LOG\n");
	for(uint32_t i = 0; i < FAIL_LOG_SIZE + 2; i++)
		printf("%x ", data[i]);
	printf("\n");
	printf("ADDRESS %x\n", address);
#endif
#if !FIRST_FLASH
	kp_btn_enable(); //unblock buttons
#endif
}


void kp_read_logs_from_flash(struct kp_lock *keypad, bool *successed)
{
#if !FIRST_FLASH
	kp_btn_disable(); //block buttons just in case
#endif

	uint32_t address = sk_search(&fail_log_sector, FAIL_LOG_SIZE + 2, false);
	if(address == NULL);

	volatile uint8_t data[FAIL_LOG_SIZE + 2];
	sk_flash_read(&data[0], FAIL_LOG_SIZE + 2, address);

	__DMB(); //Memory barrier
	unpack_fail_log(&data[0], keypad);
	if(data[FAIL_LOG_SIZE] == 0xff)
		successed = false;
	else
		successed = true;

#if SEMIHOSTING_USE
	printf("Read FAIL LOG\n");
	for(uint32_t i = 0; i < FAIL_LOG_SIZE + 2; i++)
		printf("%x ", data[i]);
	printf("\n");
	printf("ADDRESS %x\n", address);
#endif
#if !FIRST_FLASH
	kp_btn_enable(); //unblock buttons
#endif
}

void kp_discard_logs_in_flash(void)
{
#if !FIRST_FLASH
	kp_btn_disable(); //block buttons just in case
#endif

	uint32_t address = sk_search(&fail_log_sector, FAIL_LOG_SIZE + 2, false);
	if(address == NULL);
	uint8_t tag = 0x00;
	//*(uint8_t *)(address) = 0x00;
	sk_flash_write(&tag, 1, address + FAIL_LOG_SIZE);
	
#if SEMIHOSTING_USE
	printf("Discard FAIL LOG\n");
	for(uint32_t i = 0; i < FAIL_LOG_SIZE + 2; i++)
		printf("%x ", *((uint8_t *)(address + i)));
	printf("\n");
	printf("ADDRESS %x\n", address);
#endif
	#if !FIRST_FLASH
		kp_btn_enable(); //unblock buttons
	#endif
}
