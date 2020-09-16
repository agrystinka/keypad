#include "embedded_flash.h"
#include <stdio.h>

sk_err sk_flash_erase_sector(struct sk_sector *sector)
{
	if(sector == NULL)
		return SK_EWRONGARG;

    flash_erase_sector(SECTOR_NUM_11, 0);//sector - 11, program size - 8bit (0)

    //TODO: check if was erased
    
	return SK_EOK;
}


sk_err sk_flash_write(data_t *buffer, uint32_t size, data_t *address)
{
	if(buffer == NULL || address == NULL)
		return SK_EWRONGARG;

    flash_program(address, (uint8_t *)buffer, size * (sizeof(data_t)));
	return SK_EOK;
}


sk_err sk_flash_read(data_t *buffer, uint32_t size, data_t *address)
{
	if(buffer == NULL || address == NULL)
		return SK_EWRONGARG;

    for(uint32_t i = 0; i < size; i++)
        buffer[i] = *(address + i * (sizeof(buffer[i])));
	return SK_EOK;
}


bool sk_flash_empty(data_t *buffer, uint32_t size)
{
	for(uint32_t i = 0; i < size; i++)
		if(buffer[i] & EMPTY_DATA_T != EMPTY_DATA_T)
			return false;
	return true;
}


data_t* sk_find_space(struct sk_sector *sector, uint32_t size)
{
    if(sector == NULL)
		return NULL;

    if(size * (sizeof (data_t)) > sector->size )
        return NULL;

	data_t test[size]; //Atention!!!
	uint32_t count = 0; //in data_t elements
	bool is_empty = false;

	while(!is_empty && (count + size) * (sizeof (data_t)) <= sector->size){
		is_empty = false;
		sk_flash_read(test, size, sector->start + count);

		if (sk_flash_empty(test, size))
			is_empty = true;

		count += size;
	}

	if(is_empty)
		return sector->start + count;
	return NULL;
}
