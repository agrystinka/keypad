/**
 * errors - provides definitions for all error codes in buff_lib
 *
 * User may check corresponding error codes against this definitions.
 * It is not requried to include this file in user code as it is already included
 * everywhere these error definitions are used (unless you want to use same codes for own routines)
 */
 #ifndef BL_ERRORS_H
 #define BL_ERRORS_H

//If LOG == 1 put logging in stderr, else - no loging
 #define LOG 1

 enum bl_err {
 	/** No error, everything fine */
 	BL_EOK = 0,
 	/** Unknown error. Something went wrong but don't boter detecting what */
 	BL_EUNKNOWN = -1,
 	/** Some feature is not implemented yet */
 	BL_ENOTIMPLEMENTED = -2,
 	/** Something simply could not be done */
 	BL_EUNAVAILABLE = -3,
 	/** Wrong argument passed */
 	BL_EWRONGARG = -4,
 	/** BL_ENENARG */
 	BL_ENENARG = -5,
  /** To much args */
  BL_ERANGE = -6,
  /** There is too few place in buffer to alocate your data
    * or buffer is already full */
  BL_EFULL = -7,
  /** FIFO buffer is empty */
  BL_EEMPTY = -8
 };

// alias type for more concise definitions
typedef enum bl_err bl_err;

//If LOG == 1 put logging in stderr, else - no loging
#if LOG == 1
  #define _log(msg) do{ fprintf(stderr, msg); } while(0);
#else
  #define _log(msg) do{} while(0);
#endif

void bl_catch_err(bl_err err);

#endif
