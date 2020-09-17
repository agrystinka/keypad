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
        buffer[i] = (uint8_t *) (address + i);
	return SK_EOK;
}


bool sk_flash_empty(uint8_t *buffer, uint32_t size)
{
	for(uint32_t i = 0; i < size; i++)
		if(buffer[i] & EMPTY_BYTE != EMPTY_BYTE)
			return false;
	return true;
}


uint32_t sk_find_space(struct sk_sector *sector, uint32_t size)
{
    if(sector == NULL)
		return NULL;

    if(size > sector->size )
        return NULL;

	uint8_t test[size];
	uint32_t count = 0; //in bytes
	bool is_empty = false;

	while(!is_empty && count + size <= sector->size){
		is_empty = false;
		sk_flash_read(test, size, sector->start + count);

		if (sk_flash_empty(test, size))
			return sector->start + count;

		count += size;
	}
	return 0;
}
