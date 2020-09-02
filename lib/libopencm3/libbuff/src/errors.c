#include "errors.h"
#include <stdint.h>
#include <stdio.h>

void bl_catch_err(bl_err err)
{
  switch(err){
    case BL_ENOTIMPLEMENTED:
      _log("ERR: Some feature is not implemented yet.\n");
      break;
    case BL_EUNAVAILABLE:
      _log("ERR: Could not be done.\n");
      break;
    case BL_EWRONGARG:
      _log("ERR: Wrong argument.\n");
      break;
    case BL_ENENARG:
      _log("ERR: SK_ENENARG.\n");
      break;
    case BL_ERANGE:
      _log("ERR: To much args.\n");
      break;
    case BL_EFULL:
      _log("ERR: There is too few place in buffer to alocate your data.\n");
      break;
    case BL_EEMPTY:
      _log("ERR: FIFO buffer is empty.\n");
      break;
    default:
      _log("ERR: Unknown error.\n");
    }
}
