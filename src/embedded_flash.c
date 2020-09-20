#include "embedded_flash.h"
#include "intrinsics.h"
#include <libopencm3/stm32/crc.h>
#include <stdio.h>

//TODO: rewrite for void*
uint32_t sk_crc(uint8_t *data, uint32_t size)
{
	//reset crc
	 crc_reset();
	 uint32_t res;
	 if(size / (sizeof(uint32_t)) > 0) //just in case
	 	//send first size / 4 bytes to crc
	 	res = crc_calculate_block((uint32_t *)data, size / (sizeof(uint32_t)));
	 //send rest few bytes if they exist, with pading bytes equal to 0xff
	 if(size % (sizeof(uint32_t)) != 0){
		 uint32_t tmp = 0xffffffff;
		 uint8_t *pointer;
		 pointer = (uint8_t *) &tmp;
		 for(uint8_t i = 0; i < size % (sizeof(uint32_t)); i++)
		 	*pointer &= data[size / (sizeof(uint32_t)) + i];
		 res = crc_calculate(tmp);
	 }
	 return res;
}

sk_err sk_erase(struct sk_sector *sector)
{
	if(sector == NULL)
		return SK_EWRONGARG;
	flash_unlock();
    flash_erase_sector(sector->num, 0);//sector, program size - 8bit (0)
	flash_lock();
    //TODO: check if was erased

	return SK_EOK;
}


sk_err sk_flash_write(uint8_t *buffer, uint32_t size, uint32_t address)
{
	//TODO: check if adres is in flash memory

	if(buffer == NULL)
		return SK_EWRONGARG;
    //unlock flash memory to be able write to it
    flash_unlock();
    //Check that no main Flash memory operation is ongoing
    //set PG to FLASH_CR, perform the data write operation
    flash_program(address, buffer, size);
    //lock flash memoty to prevent spurious writes
    flash_lock();
    return SK_EOK;
}


sk_err sk_flash_read(uint8_t *buffer, uint32_t size, uint32_t address)
{
	//TODO: check if adres is in flash memory

	if(buffer == NULL)
		return SK_EWRONGARG;

    for(uint32_t i = 0; i < size; i++)
        buffer[i] = *((uint8_t *) (address + i));
	return SK_EOK;
}


bool sk_flash_empty(uint8_t *buffer, uint32_t size)
{
	for(uint32_t i = 0; i < size; i++)
		if(buffer[i] != EMPTY_BYTE)
			return false;
	return true;
}


uint32_t sk_search(struct sk_sector *sector, uint32_t size, bool write)
{
	//check if correct input
	if(sector == NULL || size > sector->size)
		return 0;

	uint32_t shift = 0;

	while(shift <= sector->size){
		uint8_t empty = true;
		for(uint32_t i = 0; i < size; i++)
			if(*(uint8_t *)(sector->start + shift + i) != EMPTY_BYTE)
				empty = false;

		if(empty == true)
		 	if(write == true && shift + size <= sector->size)
				return sector->start + shift;
			else if(write == false && shift >= size)
				return sector->start + shift - size;

		shift += size;
	}
	return 0;
}

//TODO: test properly and make this funcion atomic
sk_err sk_refresh(struct sk_sector *sector, struct sk_sector *sectornew, uint32_t size)
{
	if(sector == NULL || sectornew == NULL || size > sector->size || size > sectornew->size)
		return SK_EWRONGARG;
	uint32_t address = sk_search(sector, size, false);
	uint8_t data[size];

	if(sk_flash_read(&data[0], size, address) != SK_EOK)
		return SK_EUNKNOWN;
	__DMB(); //Memory barrier
	if(sk_flash_write(&data[0], size, sectornew->start) != SK_EOK)
		return SK_EUNKNOWN;
	if(flash_erase_sector(sector->num, 0) != SK_EOK)
		return SK_EUNKNOWN;

	struct sk_sector tmp = *sector;
	*sector = *sectornew;
	*sectornew = tmp;
	return SK_EOK;
}
