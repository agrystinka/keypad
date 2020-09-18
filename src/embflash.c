#include "embflash.h"
#include "pack.h"
#include "embedded_flash.h"
#include <stdio.h>

//1111 1110 attempt to unlock
#define ATTEMPT 0xfe
//1111 1100 succesful attempt to unlock
#define SUCCESSED_ATTEMPT 0xfc
//1111 1010 attempt to unlock has not been punished yet
#define FAILD_ATTEMPT_NP 0xfa
//1111 0010 attempt to unlock has been punished
#define FAILD_ATTEMPT_P 0xf2

//abstractions over flash memory sectors
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

kp_err kp_flash_init()
{
//	sk_erase(&settings_sector);
	sk_erase(&fail_log_sector);
//	sk_erase(&reserved_sector);
	return KP_OK;
}


kp_err kp_write_settings_to_flash(struct kp_lock *keypad)
{
#if !FIRST_FLASH
	kp_btn_disable(); //block buttons just in case
#endif
	volatile uint8_t data[SETTINGS_SIZE];
	pack_settings(&data[0], keypad);//Pack keypad settings to array
	uint32_t address = sk_search(&settings_sector, SETTINGS_SIZE, true);
	//TODO: test properly
	if(address == NULL){ //if there is no free space in sector
		if(sk_refresh(&fail_log_sector, &reserved_sector, SETTINGS_SIZE) != SK_EOK)
			return KP_ERR;
		address = sk_search(&fail_log_sector, SETTINGS_SIZE, true);
		if(address == NULL)
			return KP_ERR;
	}
	__DMB(); //Memory barrier
	if(sk_flash_write(&data[0], SETTINGS_SIZE, address) != SK_EOK)
		return KP_ERR;

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
	return KP_OK;
}


kp_err kp_read_settings_from_flash(struct kp_lock *keypad)
{
#if !FIRST_FLASH
	kp_btn_disable(); //block buttons just in case
#endif

	uint32_t address = sk_search(&settings_sector, SETTINGS_SIZE, false);
	if(address == NULL)
		return KP_ERR;

	volatile uint8_t data[SETTINGS_SIZE];
	if(sk_flash_read(&data[0], SETTINGS_SIZE, address) != SK_EOK)
		return KP_ERR;

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
	return KP_OK;
}

kp_err kp_write_logs_to_flash(struct kp_lock *keypad)
{
#if !FIRST_FLASH
	kp_btn_disable(); //block buttons just in case
#endif

	volatile uint8_t data[FAIL_LOG_SIZE + 1];
	pack_fail_log(&data[0], keypad);
	data[FAIL_LOG_SIZE + 0] = ATTEMPT;
	uint32_t address = sk_search(&fail_log_sector, FAIL_LOG_SIZE + 1, true);
	//TODO: test properly
	if(address == NULL){ //if there is no free space in sector, refresh it
		if(sk_refresh(&fail_log_sector, &reserved_sector, FAIL_LOG_SIZE + 1) != SK_EOK)
			return KP_ERR;
		address = sk_search(&fail_log_sector, FAIL_LOG_SIZE + 1, true);
		if(address == NULL)
			return KP_ERR;
	}

	__DMB(); //Memory barrier
	if(sk_flash_write(&data[0], FAIL_LOG_SIZE + 1, address) != SK_EOK)
		return KP_ERR;

#if SEMIHOSTING_USE
	printf("Write FAIL LOG\n");
	for(uint32_t i = 0; i < FAIL_LOG_SIZE + 1; i++)
		printf("%x ", data[i]);
	printf("\n");
	printf("ADDRESS %x\n", address);
#endif
#if !FIRST_FLASH
	kp_btn_enable(); //unblock buttons
#endif
	return KP_OK;
}

kp_err kp_read_logs_from_flash(struct kp_lock *keypad)
{
#if !FIRST_FLASH
	kp_btn_disable(); //block buttons just in case
#endif

	uint32_t address = sk_search(&fail_log_sector, FAIL_LOG_SIZE + 1, false);
	if(address == NULL)
		return KP_ERR;

	volatile uint8_t data[FAIL_LOG_SIZE + 1];
	if(sk_flash_read(&data[0], FAIL_LOG_SIZE + 1, address) != SK_EOK)
		return KP_ERR;

	__DMB(); //Memory barrier

	unpack_fail_log(&data[0], keypad);

#if SEMIHOSTING_USE
	printf("Read FAIL LOG\n");
	for(uint32_t i = 0; i < FAIL_LOG_SIZE + 1; i++)
		printf("%x ", data[i]);
	printf("\n");
	printf("ADDRESS %x\n", address);
#endif
#if !FIRST_FLASH
	kp_btn_enable(); //unblock buttons
#endif
	return KP_OK;
}


bool kp_if_failed_logs(void)
{
	uint32_t address = sk_search(&fail_log_sector, FAIL_LOG_SIZE + 1, false);
	volatile uint8_t tag;
	sk_flash_read(&tag, 1, address + FAIL_LOG_SIZE);
	__DMB(); //Memory barrier

#if SEMIHOSTING_USE
	printf("Tag: %x ", tag);
#endif

	if(tag == FAILD_ATTEMPT_NP || tag == FAILD_ATTEMPT_P) //if logs are important
		return true;
	else
		return false;
}

//if return true - need to be punished
bool kp_if_failed_logs_np(void)
{
	uint32_t address = sk_search(&fail_log_sector, FAIL_LOG_SIZE + 1, false);
	volatile uint8_t tag;
	sk_flash_read(&tag, 1, address + FAIL_LOG_SIZE);
	__DMB(); //Memory barrier

#if SEMIHOSTING_USE
	printf("Tag: %x ", tag);
#endif

	if(tag == FAILD_ATTEMPT_NP) //if need to be punished
		return true;
	else
		return false;
}


static kp_err kp_logs_tag_recent(uint8_t tag)
{
#if !FIRST_FLASH
	kp_btn_disable(); //block buttons just in case
#endif

	uint32_t address = sk_search(&fail_log_sector, FAIL_LOG_SIZE + 1, false);
	//TODO: test properly
	if(address == NULL){ //if there is no free space in sector, refresh it
		if(sk_refresh(&fail_log_sector, &reserved_sector, FAIL_LOG_SIZE + 1) != SK_EOK)
			return KP_ERR;
		address = sk_search(&fail_log_sector, FAIL_LOG_SIZE + 1, true);
		if(address == NULL)
			return KP_ERR;
	}
	uint8_t data = tag;
	if(sk_flash_write(&data, 1, address + FAIL_LOG_SIZE) != SK_EOK)
		return KP_ERR;

#if !FIRST_FLASH
	kp_btn_enable(); //unblock buttons
#endif
	return KP_OK;
}


void kp_logs_in_flash_successed(void)
{
	kp_logs_tag_recent(SUCCESSED_ATTEMPT);
}


void kp_logs_in_flash_failed(void)
{
	kp_logs_tag_recent(FAILD_ATTEMPT_NP);
}


void kp_logs_in_flash_failed_p(void)
{
	kp_logs_tag_recent(FAILD_ATTEMPT_P);
}
