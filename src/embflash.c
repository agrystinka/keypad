#include "keypad.h"

#define GLOBAL_DATA_SIZE 9
#define ADRESS_FLASH 0x080E0000
//#define SECTOR_FLASH 2

void pack_global_data(uint32_t *buffer)
{
	//uint32_t *pointer32 = buffer;
	//uint8_t *pointer8 = (uint8_t *) buffer;
	//pack delays  (uint32_t variables) 4 * 3 = 12 bytes
	buffer[0] = WELCOME_DELAY_S;
	buffer[1] = FAIL_DELAY_S;
	buffer[2] = FAIL_DELAY_CUR_S;
	//pack uint8_t variables
	//pack passwords: 2 * 8  = 16 bytes(if MAX_PASS_LENGTH == 8)
	uint8_t *pointer8 = (uint8_t *) &buffer[3];
	for(int i = 0; i < MAX_PASS_LENGTH; i++)
		*(pointer8 + i) = USR_PASS[i];
	pointer8 += MAX_PASS_LENGTH;
	for(int i = 0; i < MAX_PASS_LENGTH; i++)
		*(pointer8 + i) = MASTER_CODE[i];
	pointer8 += MAX_PASS_LENGTH;
	//pack pass kp_set_length: 2 bytes
	*(pointer8 + 0) = USR_PASS_LENGTH;
	*(pointer8 + 1) = MASTER_CODE_LENGTH;
	//pack settings: 5 bytes
	*(pointer8 + 2) = FAILS;
	*(pointer8 + 3) = CRYTICAL_FAILS_LOW;
	*(pointer8 + 4) = CRYTICAL_FAILS_HIGHT;
	*(pointer8 + 5) = DELAY_COEFF;
	if(KP_MODE)
		*(pointer8 + 6) = 1;
	else
		*(pointer8 + 6) = 0;
	//total: 12 + 16 + 2 + 5 = 35 byte = 9 words
}


void unpack_global_data(uint32_t *buffer)
{
	//uint32_t *pointer32 = buffer;
	//uint8_t *pointer8 = (uint8_t *) buffer;
	//pack delays  (uint32_t variables) 4 * 3 = 12 bytes
	WELCOME_DELAY_S = buffer[0];
	FAIL_DELAY_S = buffer[1];
	FAIL_DELAY_CUR_S = buffer[2];
	//pack uint8_t variables
	//pack passwords: 2 * 8  = 16 bytes(if MAX_PASS_LENGTH == 8)
	uint8_t *pointer8 = (uint8_t *) &buffer[3];
	for(int i = 0; i < MAX_PASS_LENGTH; i++)
		USR_PASS[i] = *(pointer8 + i);
	pointer8 += MAX_PASS_LENGTH;
	for(int i = 0; i < MAX_PASS_LENGTH; i++)
		MASTER_CODE[i] = *(pointer8 + i);
	pointer8 += MAX_PASS_LENGTH;
	//pack pass kp_set_length: 2 bytes
	USR_PASS_LENGTH = *(pointer8 + 0);
	MASTER_CODE_LENGTH = *(pointer8 + 1);
	//pack settings: 5 bytes
	FAILS = *(pointer8 + 2);
	CRYTICAL_FAILS_LOW = *(pointer8 + 3);
	CRYTICAL_FAILS_HIGHT = *(pointer8 + 4);
	DELAY_COEFF = *(pointer8 + 5);
	if(*(pointer8 + 6))
		KP_MODE = true;
	else
		KP_MODE = false;
	//total: 12 + 16 + 2 + 5 = 35 byte = 9 words
}


void write_global_data_to_flash(void)
{
	uint32_t data[GLOBAL_DATA_SIZE];
	//pack_global_data(&data[0]);
    //rewrite_flash(uint32_t *buffer, uint32_t size);

    //erase sector befor writing to it
	flash_unlock();
    flash_erase_sector(11, 0);//sector - 0, program size - 8bit (0)
    //Check that no main Flash memory operation is ongoing
    //flash_wait_for_last_operation();
	pack_global_data(&data[0]);

    //set PG to FLASH_CR,
    //perform the data write operation (32 bits)
    //wait for the BSY bit to be cleared.
    volatile uint32_t addr = ADRESS_FLASH; //11 sector
    for(uint32_t i = 0; i < GLOBAL_DATA_SIZE; i++){
        flash_program_word(addr, data[i]);
        //adrr += 4;
        addr += sizeof(data[i]);
    }
	flash_lock();
}


void read_global_data_from_flash(void)
{
	uint32_t data[GLOBAL_DATA_SIZE];
	//pack_global_data(&data[0]);
    // read data from flash
    uint32_t *addr;
    addr = ADRESS_FLASH;
    for(uint32_t i = 0; i < GLOBAL_DATA_SIZE; i++){
        data[i] = *addr;
        addr += sizeof(data[i]);
		delay_ms_systick(1);
    }
	WELCOME_DELAY_S = 10;
	for(uint32_t i = 0; i < GLOBAL_DATA_SIZE; i++){
        if(data[i] != 0xffffffff)
			WELCOME_DELAY_S = 5;
    }
    //unpack_global_data(&data[0]);
}
