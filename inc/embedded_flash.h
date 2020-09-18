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
#define SECTOR_ADDRESS_4 0x08010000
#define SECTOR_ADDRESS_3 0x0800C000
#define SECTOR_ADDRESS_2 0x08008000
#define SECTOR_ADDRESS_1 0x08004000
#define SECTOR_ADDRESS_0 0x08000000

#define SECTOR_NUM_11 11
#define SECTOR_NUM_10 10
#define SECTOR_NUM_9 9
#define SECTOR_NUM_8 8
#define SECTOR_NUM_7 7
#define SECTOR_NUM_6 6
#define SECTOR_NUM_5 5
#define SECTOR_NUM_4 4
#define SECTOR_NUM_3 3
#define SECTOR_NUM_2 2
#define SECTOR_NUM_1 1
#define SECTOR_NUM_0 0

#define SECTOR_128_SIZE 0x20000 //128 Kbyte = 128 * 1024 = 131072 byte = 0x20000 byte
#define SECTOR_64_SIZE 0x10000 //64 Kbyte = 64 * 1024 = 65536 byte = 0x10000 byte
#define SECTOR_16_SIZE 0x4000 //16 Kbyte = 16 * 1024 = 16384 byte = 0x4000 byte

#define EMPTY_BYTE 0xff

struct sk_sector {
    uint32_t start; //addres of begining of sector
    uint32_t size; //in bytes
    uint8_t num; //number of sector
};

/**
 * sk_format() - erase sector.
 * @struct sk_sector *sector: sector of flash memory to search.
 *
 * Return: sk_err.
 */
sk_err sk_erase(struct sk_sector *sector);
/**
 * sk_flash_write() - write data from buffer to flash memory.
 * @uint8_t *buffer: buffer with data.
 * @uint32_t size: size of buffer.
 * @uint32_t address: adress in flash memory from which start writing data.
 *
 * Return: sk_err.
 */
sk_err sk_flash_write(uint8_t *buffer, uint32_t size, uint32_t address);

/**
 * sk_flash_read() - read data from flash memory to buffer.
 * @uint8_t *buffer: buffer to store read data.
 * @uint32_t size: size of buffer.
 * @uint32_t address: adress in flash memory from which start to read data.
 *
 * Return: sk_err.
 */
sk_err sk_flash_read(uint8_t *buffer, uint32_t size, uint32_t address);

/**
 * sk_flash_empty() - check if flash memory is empty.
 * @uint8_t  *buffer: buffer to check.
 * @uint32_t size: size of buffer.
 *
 * Check if all bits of buffer are equal to 1.
 *
 * Return: bool.
 * Return true if memory is empty, false if not.
 */
bool sk_flash_empty(uint8_t *buffer, uint32_t size);

/**
 * sk_search() - find empty space with chosen size in flash memory.
 * @struct sk_sector *sector: sector of flash memory to search.
 * @uint32_t size: size of required empty space in data_t elements (sizeof(data_t) bytes).
 * @bool write: if true - return adress on start of empty space of given size,
 *              if false - return adress on start of last note with given size.
 *
 * Return: uint32_t.
 */
uint32_t sk_search(struct sk_sector *sector, uint32_t size, bool write);

/**
 * sk_refresh() - refresh flash memory sector.
 * @struct sk_sector *sector: old sector.
 * @struct sk_sector *sectornew: new sector (must be empty).
 * @uint32_t size: size of note.
 *
 * Copy recent note from sector to sectornew, erase sector, tag them.
 * In the resuls sector has only recent note, sector new is empty.
 * Return: sk_err.
 */
sk_err sk_refresh(struct sk_sector *sector, struct sk_sector *sectornew, uint32_t size);
