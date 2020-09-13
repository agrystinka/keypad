#include "first_flash.h"

// #define GLOBAL_DATA_SIZE 9
#define GLOBAL_DATA_SIZE 35
#define ADRESS_FLASH 0x080E0000
#define SECTOR_FLASH 11

/////////////----------8----------------------------------------------------------
void pack_global_data(uint8_t *buffer)
{
	uint32_t *pointer32 = (uint32_t *)buffer;
	//pack delays  (uint32_t variables) 4 * 3 = 12 bytes
	uint32_t shift = 12;
	*(pointer32 + 0) = WELCOME_DELAY_S;
	*(pointer32 + 1) = FAIL_DELAY_S;
	*(pointer32 + 2) = FAIL_DELAY_CUR_S;
	//pack uint8_t variables
	//pack passwords: 2 * 8  = 16 bytes(if MAX_PASS_LENGTH == 8)
	for(int i = 0; i < MAX_PASS_LENGTH; i++)
		buffer[shift + i] = USR_PASS[i];
	shift += MAX_PASS_LENGTH;
	for(int i = 0; i < MAX_PASS_LENGTH; i++)
		buffer[shift + i] = MASTER_CODE[i];
	shift += MAX_PASS_LENGTH;
	//pack pass kp_set_length: 2 bytes
	buffer[shift + 0] = USR_PASS_LENGTH;
	buffer[shift + 1] = MASTER_CODE_LENGTH;
	//pack settings: 5 bytes
	buffer[shift + 2] = FAILS;
	buffer[shift + 3] = CRYTICAL_FAILS_LOW;
	buffer[shift + 4] = CRYTICAL_FAILS_HIGHT;
	buffer[shift + 5] = DELAY_COEFF;
	if(KP_MODE)
		buffer[shift + 6] = 1;
	else
		buffer[shift + 6] = 0;
	//total: 12 + 16 + 2 + 5 = 35 byte = 9 words
}


void unpack_global_data(uint8_t *buffer)
{
	uint32_t *pointer32 = (uint32_t *)buffer;
	//pack delays  (uint32_t variables) 4 * 3 = 12 bytes
	uint32_t shift = 12;
	WELCOME_DELAY_S = *(pointer32 + 0);
	FAIL_DELAY_S = *(pointer32 + 1);
	FAIL_DELAY_CUR_S = *(pointer32 + 2);
	//pack uint8_t variables
	//pack passwords: 2 * 8  = 16 bytes(if MAX_PASS_LENGTH == 8)
	uint8_t *pointer8 = (uint8_t *) &buffer[3];
	for(int i = 0; i < MAX_PASS_LENGTH; i++)
		USR_PASS[i] = buffer[shift + i];
	shift += MAX_PASS_LENGTH;
	for(int i = 0; i < MAX_PASS_LENGTH; i++)
		MASTER_CODE[i] = buffer[shift + i];
	shift += MAX_PASS_LENGTH;
	//pack pass kp_set_length: 2 bytes
	USR_PASS_LENGTH = buffer[shift + 0];
	MASTER_CODE_LENGTH = buffer[shift + 1];
	//pack settings: 5 bytes
	FAILS = buffer[shift + 3];
	CRYTICAL_FAILS_LOW = buffer[shift + 3];
	CRYTICAL_FAILS_HIGHT =buffer[shift + 4];
	DELAY_COEFF = buffer[shift + 5];
	if(buffer[shift + 6])
		KP_MODE = true;
	else
		KP_MODE = false;
	//total: 12 + 16 + 2 + 5 = 35 byte = 9 words
}


void write_global_data_to_flash(void)
{
	volatile uint8_t data[GLOBAL_DATA_SIZE];
    //unlock flash memory to be able write to it
	flash_unlock();
	//erase sector befor writing to it
    flash_erase_sector(11, 0);//sector - 11, program size - 8bit (0)
	//Pack global variables to array
	pack_global_data(&data[0]);
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
}


void read_global_data_from_flash(void)
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
	unpack_global_data(&data[0]);
}
/////////////----------32---------------------------------------------------------
//
// void pack_global_data(uint32_t *buffer)
// {
// 	//uint32_t *pointer32 = buffer;
// 	//uint8_t *pointer8 = (uint8_t *) buffer;
// 	//pack delays  (uint32_t variables) 4 * 3 = 12 bytes
// 	buffer[0] = WELCOME_DELAY_S;
// 	buffer[1] = FAIL_DELAY_S;
// 	buffer[2] = FAIL_DELAY_CUR_S;
// 	//pack uint8_t variables
// 	//pack passwords: 2 * 8  = 16 bytes(if MAX_PASS_LENGTH == 8)
// 	uint8_t *pointer8 = (uint8_t *) &buffer[3];
// 	for(int i = 0; i < MAX_PASS_LENGTH; i++)
// 		*(pointer8 + i) = USR_PASS[i];
// 	pointer8 += MAX_PASS_LENGTH;
// 	for(int i = 0; i < MAX_PASS_LENGTH; i++)
// 		*(pointer8 + i) = MASTER_CODE[i];
// 	pointer8 += MAX_PASS_LENGTH;
// 	//pack pass kp_set_length: 2 bytes
// 	*(pointer8 + 0) = USR_PASS_LENGTH;
// 	*(pointer8 + 1) = MASTER_CODE_LENGTH;
// 	//pack settings: 5 bytes
// 	*(pointer8 + 2) = FAILS;
// 	*(pointer8 + 3) = CRYTICAL_FAILS_LOW;
// 	*(pointer8 + 4) = CRYTICAL_FAILS_HIGHT;
// 	*(pointer8 + 5) = DELAY_COEFF;
// 	if(KP_MODE)
// 		*(pointer8 + 6) = 1;
// 	else
// 		*(pointer8 + 6) = 0;
// 	//total: 12 + 16 + 2 + 5 = 35 byte = 9 words
// }
//
//
// void unpack_global_data(uint32_t *buffer)
// {
// 	//uint32_t *pointer32 = buffer;
// 	//uint8_t *pointer8 = (uint8_t *) buffer;
// 	//pack delays  (uint32_t variables) 4 * 3 = 12 bytes
// 	WELCOME_DELAY_S = buffer[0];
// 	FAIL_DELAY_S = buffer[1];
// 	FAIL_DELAY_CUR_S = buffer[2];
// 	//pack uint8_t variables
// 	//pack passwords: 2 * 8  = 16 bytes(if MAX_PASS_LENGTH == 8)
// 	uint8_t *pointer8 = (uint8_t *) &buffer[3];
// 	for(int i = 0; i < MAX_PASS_LENGTH; i++)
// 		USR_PASS[i] = *(pointer8 + i);
// 	pointer8 += MAX_PASS_LENGTH;
// 	for(int i = 0; i < MAX_PASS_LENGTH; i++)
// 		MASTER_CODE[i] = *(pointer8 + i);
// 	pointer8 += MAX_PASS_LENGTH;
// 	//pack pass kp_set_length: 2 bytes
// 	USR_PASS_LENGTH = *(pointer8 + 0);
// 	MASTER_CODE_LENGTH = *(pointer8 + 1);
// 	//pack settings: 5 bytes
// 	FAILS = *(pointer8 + 2);
// 	CRYTICAL_FAILS_LOW = *(pointer8 + 3);
// 	CRYTICAL_FAILS_HIGHT = *(pointer8 + 4);
// 	DELAY_COEFF = *(pointer8 + 5);
// 	if(*(pointer8 + 6))
// 		KP_MODE = true;
// 	else
// 		KP_MODE = false;
// 	//total: 12 + 16 + 2 + 5 = 35 byte = 9 words
// }
//
//
// void write_global_data_to_flash(void)
// {
// 	uint32_t data[GLOBAL_DATA_SIZE];
//     //erase sector befor writing to it
// 	flash_unlock();
//     flash_erase_sector(11, 0);//sector - 11, program size - 8bit (0)
//     //Check that no main Flash memory operation is ongoing
// 	pack_global_data(&data[0]);
//     //set PG to FLASH_CR,
//     //perform the data write operation (32 bits)
//     //wait for the BSY bit to be cleared.
//     volatile uint32_t addr = ADRESS_FLASH; //11 sector
//     for(uint32_t i = 0; i < GLOBAL_DATA_SIZE; i++){
//         flash_program_word(addr, data[i]);
//         addr += sizeof(data[i]);
//     }
// 	flash_lock();
// }
//
//
// void read_global_data_from_flash(void)
// {
// 	uint32_t data[GLOBAL_DATA_SIZE];
//     uint32_t *addr;
//     addr = ADRESS_FLASH;
//     for(uint32_t i = 0; i < GLOBAL_DATA_SIZE; i++){
//         data[i] = *addr;
//         addr += sizeof(data[i]);
// 		delay_ms_systick(1);
//     }
// }
