#ifndef KP_CMD_H
#define KP_CMD_H

#include <stdint.h>

enum kp_buttons{
    KP_NONE     = 0,
    KP_UP       = 1,
    KP_DOWN     = 2,
    KP_RIGHT    = 3,
    KP_LEFT     = 4,
    KP_MENU     = 5
};

//must be initialized as global variable    
extern uint8_t KP_CMD;

#endif
