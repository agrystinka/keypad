#include "errors.h"
#include <stdint.h>
#include <stdbool.h>

#define SECTOR_ADDRESS_11 0x080E0000
#define SECTOR_ADDRESS_10 0x080C0000
#define SECTOR_ADDRESS_9 0x080A0000
#define SECTOR_ADDRESS_8 0x08080000
#define SECTOR_ADDRESS_7 0x08060000
#define SECTOR_ADDRESS_6 0x08040000
#define SECTOR_ADDRESS_5 0x08020000

#define SECTOR_NUM_11 11
#define SECTOR_NUM_10 10
#define SECTOR_NUM_9 9
#define SECTOR_NUM_8 8
#define SECTOR_NUM_7 7
#define SECTOR_NUM_6 6
#define SECTOR_NUM_5 5

// 128 Kbyte = 128 * 1024 = 131072 byte = 0x20000 byte
#define SECTOR_128_SIZE 0x20000

#define EMPTY_BYTE 0xff

struct sk_sector {
    uint32_t start; //addres of begining of sector
    uint32_t size; //in bytes
    uint8_t num; //number of sector
    //bool isempty;
};

/**
 * sk_flash_write() - write data from buffer to flash memory.
 * @data_t *buffer: buffer with data.
 * @uint32_t size: size of buffer (size of data to read).
 *                 Equal to size * (sizeof(data_t )) bytes.
 * data_t *address: adress in flash memory from which start writing data.
 *
 * Return: sk_err.
 */
sk_err sk_flash_write(uint8_t *buffer, uint32_t size, uint32_t address);

/**
 * sk_flash_read() - read data from flash memory to buffer.
 * @data_t *buffer: buffer to store read data.
 * @uint32_t size: size of buffer (size of data to read).
 *                 Equal to size * (sizeof(data_t )) bytes.
 * data_t *address: adress in flash memory from which start to read data.
 *
 * Return: sk_err.
 */
sk_err sk_flash_read(uint8_t *buffer, uint32_t size, uint32_t address);

/**
 * sk_flash_empty() - check if flash memory is empty.
 * @data_t  *buffer: buffer to check.
 * @uint32_t size: size of buffer.
 *
 * Check if all bits of buffer are equal to 1.
 *
 * Return: bool.
 * Return true if memory is empty, false if not.
 */
bool sk_flash_empty(uint8_t *buffer, uint32_t size);

/**
 * sk_find_space() - find empty space with chosen size in flash memory.
 * @struct sk_sector *sector: sector of flash memory to search.
 * @uint32_t size: size of required empty space in data_t elements (sizeof(data_t) bytes).
 *
 * Return: data_t* .
 * Return pointer on start of this empty space of chosen size.
 */
uint32_t sk_find_space(struct sk_sector *sector, uint32_t size);
