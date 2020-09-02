#ifndef BUFF_LIB_H
#define BUFF_LIB_H

#include "errors.h"
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * struct bl_bfifo - FIFO buffer.
 * @uint8_t *buf: buffer for data.
 * @uint32_t buflen: lenth of buffer for data.
 * @uint32_t rdidx: index of next read element.
 * @uint32_t wridx: index of next written element.
 * @uint8_t isfull: (1 bit) true if buffer is full, falseis not.
 */
struct bl_bfifo {
	uint8_t *buf;
	uint32_t buflen;
	uint32_t rdidx;
	uint32_t wridx;
	uint8_t isfull : 1;
};

typedef struct bl_bfifo bl_bfifo_t;

/**
 * bl_bfifo_init() - initialization of FIFO buffer.
 * @bl_bfifo_t *fifo: struct of FIFO buffer.
 * @uint8_t *buf: allocated (by user) buffer.
 * @uint32_t buflen: lenth of buffer.
 *
 * Return: bl_err.
 *
 * Function returns BL_EOK if it has been executed successfully.
 * For more details read errors.h.
 */
bl_err bl_bfifo_init(bl_bfifo_t *fifo, uint8_t *buf, uint32_t buflen);

/**
 * bl_bfifo_cur_length() - get current buffer length.
 * @bl_bfifo_t *fifo: struct of FIFO buffer.
 *
 * Return: uint32_t.
 *
 * Function returns length of current buffered data.
 */
uint32_t bl_bfifo_cur_length(bl_bfifo_t *fifo);

/**
 * bl_bfifo_put() - put (write) elements to FIFO buffer.
 * @bl_bfifo_t *fifo: struct of FIFO buffer.
 * @uint8_t *barr: elements which have to be put.
 * @uint32_t len: amount of elements which have to be put.
 *
 * Return: bl_err.
 *
 * Function returns BL_EOK if it has been executed successfully.
 * For more details read errors.h.
 */
bl_err bl_bfifo_put(bl_bfifo_t *fifo, uint8_t *barr, uint32_t len);

/**
 * bl_bfifo_get() - get (read) elements from FIFO buffer.
 * @bl_bfifo_t *fifo: struct of FIFO buffer.
 * @uint8_t *barr: buffer for elements which you want to get.
 * @uint32_t len: amount of elements which you want to get.
 *
 * Return: bl_err.
 *
 * Function returns BL_EOK if it has been executed successfully.
 * For more details read errors.h.
 */
bl_err bl_bfifo_get(bl_bfifo_t *fifo, uint8_t *barr, uint32_t len);

/**
 * _printfifo() - print info about FIFO buffer int stderr.
 * @bl_bfifo_t *fifo: struct of FIFO buffer.
 *
 * Return:  void.
 */
void _printfifo(bl_bfifo_t *fifo);

#endif
