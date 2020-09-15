#include "bufflib.h"

static inline bool _bl_bfifo_is_full(bl_bfifo_t *fifo)
{
	return (1 == fifo->isfull);
}

static inline bool _bl_bfifo_is_empty(bl_bfifo_t *fifo)
{
	return (!_bl_bfifo_is_full(fifo)) && (fifo->rdidx == fifo->wridx);
}

bl_err bl_bfifo_init(bl_bfifo_t *fifo, uint8_t *buf, uint32_t buflen)
{
	if ((NULL == fifo) || (NULL == buf) || (0 == buflen))
		return BL_EWRONGARG;

	*fifo = (bl_bfifo_t) {
		.buf = buf,
		.buflen = buflen,
		.rdidx = 0,
		.wridx = 0,
		.isfull = false
	};
	return BL_EOK;
}

// empty slots = buflen - X, where X is number of elements currently in buffer
// when wridx < rdidx    X = wridx - rdidx
// when wridx > rdidx    X = buflen - (rdidx - wridx) = buflen + wridx - rdidx
uint32_t bl_bfifo_cur_length(bl_bfifo_t *fifo)
{
  if (NULL == fifo)
		return BL_EUNKNOWN;

  if(fifo->wridx < fifo->rdidx)
    return fifo->buflen - (fifo->rdidx - fifo->wridx);
  else if(fifo->wridx > fifo->rdidx)
    return  fifo->wridx - fifo->rdidx;
  else if(_bl_bfifo_is_full(fifo))
    return  fifo->buflen;
  else
    return 0;
}

static bl_err _bl_bfifo_write(bl_bfifo_t *fifo, uint8_t *barr, uint32_t len)
{
  if ((NULL == fifo) || (NULL == barr) || (0 == len))
		return BL_EWRONGARG;

  for(uint32_t i = 0; i < len; i++)
    fifo->buf[(fifo->wridx + i) % fifo->buflen] = barr[i];

  fifo->wridx = (fifo->wridx + len) % fifo->buflen;

  if(fifo->wridx == fifo->rdidx)
    fifo->isfull = true;

  return BL_EOK;
}

bl_err bl_bfifo_put(bl_bfifo_t *fifo, uint8_t *barr, uint32_t len)
{
  if ((NULL == fifo) || (NULL == barr) || (0 == len))
		return BL_EWRONGARG;

  uint32_t curlenght = bl_bfifo_cur_length(fifo);
  if((bl_err) curlenght < 0)
    return curlenght;

  if (_bl_bfifo_is_full(fifo) || len + curlenght > fifo->buflen)
		return BL_EFULL;

  if(BL_EOK != _bl_bfifo_write(fifo, barr, len))
    return BL_EUNKNOWN;

  return BL_EOK;
}

static bl_err _bl_bfifo_read(bl_bfifo_t *fifo, uint8_t *barr, uint32_t len)
{
  if ((NULL == fifo) || (NULL == barr) || (0 == len))
		return BL_EWRONGARG;

  for(uint32_t i = 0; i < len; i++)
    barr[i] = fifo->buf[(fifo->rdidx + i) % fifo->buflen];

  fifo->rdidx = (fifo->rdidx + len) % fifo->buflen;
  fifo->isfull = false;

  return BL_EOK;
}

bl_err bl_bfifo_get(bl_bfifo_t *fifo, uint8_t *barr, uint32_t len)
{
  if ((NULL == fifo) || (NULL == barr) || (0 == len))
		return BL_EWRONGARG;

  uint32_t curlenght = bl_bfifo_cur_length(fifo);

  if((bl_err)curlenght < 0)
    return curlenght;

  if(curlenght == 0)
    return BL_EEMPTY;

  if(curlenght < len)
    return BL_ERANGE;

  if(BL_EOK != _bl_bfifo_read(fifo, barr, len))
    return BL_EUNKNOWN;

  return BL_EOK;
}

void _printfifo(bl_bfifo_t *fifo)
{
	if (NULL != fifo) {
		fprintf(stderr, "\t> (addr %p) BUF=%p LEN=%llu RWIDX=%llu WRIDX=%llu ISFULL=%c\n",
						fifo, fifo->buf, (unsigned long long)fifo->buflen,
						(unsigned long long)fifo->rdidx, (unsigned long long)fifo->wridx,
						(fifo->isfull ? 'T' : 'F'));
	} else {
		fprintf(stderr, "\t!! NULL\n");
	}
}
