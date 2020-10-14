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
	sk_erase(&settings_sector);
	sk_erase(&fail_log_sector);
	sk_erase(&reserved_sector);
	return KP_OK;
}


kp_err kp_write_settings_to_flash(struct kp_lock *keypad)
{
	volatile uint8_t data[SETTINGS_SIZE + CRC_SIZE];
	pack_settings(&data[0], keypad);//Pack keypad settings to array
	//calculate and pack crc
	uint32_t crc = sk_crc(&data[0], SETTINGS_SIZE);
	for(uint8_t i = 0; i < sizeof(uint32_t); i++)
		data[SETTINGS_SIZE + i] = *(((uint8_t *)&crc) + i);

	uint32_t address = sk_search(&settings_sector, SETTINGS_SIZE + CRC_SIZE, true);
	//TODO: test properly
	if(address == 0){ //if there is no free space in sector
		if(sk_refresh(&fail_log_sector, &reserved_sector, SETTINGS_SIZE) != SK_EOK)
			return KP_ERR;
		address = sk_search(&fail_log_sector, SETTINGS_SIZE, true);
		if(address == 0)
			return KP_ERR;
	}
	__DMB(); //Memory barrier
	if(sk_flash_write(&data[0], SETTINGS_SIZE + CRC_SIZE, address) != SK_EOK)
		return KP_ERR;

#if SEMIHOSTING_USE
	printf("Write DATA\n");
	for(uint32_t i = 0; i < SETTINGS_SIZE + CRC_SIZE; i++)
		printf("%x ", data[i]);
	printf("\n");
	printf("CRC: %x\n", crc);
	printf("Write DATA ADDRESS %x\n", address);
#endif
	return KP_OK;
}


kp_err kp_read_settings_from_flash(struct kp_lock *keypad)
{
	uint32_t address = sk_search(&settings_sector, SETTINGS_SIZE + CRC_SIZE, false);
	if(address == 0)
		return KP_ERR;

	volatile uint8_t data[SETTINGS_SIZE + CRC_SIZE];
	if(sk_flash_read(&data[0], SETTINGS_SIZE + CRC_SIZE, address) != SK_EOK)
		return KP_ERR;

	__DMB(); //Memory barrier
	unpack_settings(&data[0], keypad);
	uint32_t crc1 = *((uint32_t*)&data[SETTINGS_SIZE]); //unpack crc
	uint32_t crc2 = sk_crc(&data[0], SETTINGS_SIZE); //calculate crc

	if(crc1 != crc2)
		return KP_ERR;

#if SEMIHOSTING_USE
	printf("Read DATA\n");
	for(uint32_t i = 0; i < SETTINGS_SIZE + CRC_SIZE; i++)
		printf("%x ", data[i]);
	printf("\n");
	printf("CRC1: %x\n", crc1);
	printf("CRC2: %x\n", crc2);
	printf("Read DATA ADDRESS %x\n", address);
#endif
	return KP_OK;
}

kp_err kp_write_logs_to_flash(struct kp_lock *keypad)
{
	volatile uint8_t data[FAIL_LOG_SIZE + 1];
	pack_fail_log(&data[0], keypad);
	data[FAIL_LOG_SIZE + 0] = ATTEMPT;
	uint32_t address = sk_search(&fail_log_sector, FAIL_LOG_SIZE + 1, true);

	if(address == 0){ //if there is no free space in sector, refresh it
		if(sk_refresh(&fail_log_sector, &reserved_sector, FAIL_LOG_SIZE + 1) != SK_EOK)
			return KP_ERR;
		address = sk_search(&fail_log_sector, FAIL_LOG_SIZE + 1, true);
		if(address == 0)
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
	return KP_OK;
}

kp_err kp_read_logs_from_flash(struct kp_lock *keypad)
{
	uint32_t address = sk_search(&fail_log_sector, FAIL_LOG_SIZE + 1, false);
	if(address == 0)
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


bool kp_if_failed_logs_np(void)
{
	uint32_t address = sk_search(&fail_log_sector, FAIL_LOG_SIZE + 1, false);
	volatile uint8_t tag;
	sk_flash_read(&tag, 1, address + FAIL_LOG_SIZE);
	__DMB(); //Memory barrier

#if SEMIHOSTING_USE
	printf("Tag: %x ", tag);
#endif

	if(tag == FAILD_ATTEMPT_NP) //if user must be punished
		return true;
	else
		return false;
}


static kp_err kp_logs_tag_recent(uint8_t tag)
{
	uint32_t address = sk_search(&fail_log_sector, FAIL_LOG_SIZE + 1, false);

	if(address == 0){ //if there is no free space in sector, refresh it
		if(sk_refresh(&fail_log_sector, &reserved_sector, FAIL_LOG_SIZE + 1) != SK_EOK)
			return KP_ERR;
		address = sk_search(&fail_log_sector, FAIL_LOG_SIZE + 1, true);
		if(address == 0)
			return KP_ERR;
	}
	uint8_t data = tag;
	if(sk_flash_write(&data, 1, address + FAIL_LOG_SIZE) != SK_EOK)
		return KP_ERR;
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

bool kp_if_flash_empty(void)
{
	if(sk_if_sector_empty(&settings_sector) && sk_if_sector_empty(&fail_log_sector) &&
		sk_if_sector_empty(&reserved_sector))
		return true;
	return false;
}
