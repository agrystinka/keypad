#include "embedded_flash.h"
#include <stdio.h>

sk_err sk_flash_erase_sector(struct sk_sector *sector)
{
	if(sector == NULL)
		return SK_EWRONGARG;

    flash_erase_sector(sector->num, 0);//sector - 11, program size - 8bit (0)

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
		return NULL;

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
	return NULL;
}
