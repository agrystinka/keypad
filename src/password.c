//#include "keypad.h"

bool kp_check_plain(uint8_t *pass)
{
    for(uint8_t i = 0; i < PASS_LENGTH; i++)
        if(PASSWORD[i] != pass[i])
            return false;
    return true;
}
