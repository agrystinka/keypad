#ifndef KP_KEYPAD_H
#define KP_KEYPAD_H

#include "cmd.h"
#include "setup.h"
#include "screen.h"
#include "menu.h"
#include "password.h"
#include "intrinsics.h"
//#include <libopencm3/stm32/flash.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#define MAX_PASS_LENGTH   8
#define MIDLE_PASS_LENGTH 6
#define MIN_PASS_LENGTH   4

extern uint8_t USR_PASS_LENGTH;
extern uint8_t USR_PASS[MAX_PASS_LENGTH];
//To change settings
extern uint8_t MASTER_CODE_LENGTH;
extern uint8_t MASTER_CODE[MAX_PASS_LENGTH];
//To open menu(settings)
//extern uint8_t MENU_CODE_LENGTH;
extern uint8_t MENU_CODE[MAX_PASS_LENGTH];

/* 0 - switch state; 1 - single action */
extern bool KP_MODE ;

//Deleys and secure settings
extern uint32_t WELCOME_DELAY_S;
extern uint32_t FAIL_DELAY_S;
extern uint32_t FAIL_DELAY_CUR_S;
extern uint8_t FAILS;
extern uint8_t CRYTICAL_FAILS_LOW;
extern uint8_t CRYTICAL_FAILS_HIGHT;
extern uint8_t DELAY_COEFF;

extern uint8_t STATE_SYMBOL;
extern uint8_t KP_CMD;

struct keypad {
    uint8_t usrpass_length;
    uint8_t usrpass[MAX_PASS_LENGTH];
    uint8_t mstrpass_length;
    uint8_t mstrpass[MAX_PASS_LENGTH];
    uint8_t menucode[MAX_PASS_LENGTH];
    uint32_t delay_open_s;
    uint32_t delay_wait_s;
    uint32_t delay_wait_cur_s;
    uint8_t fails;
    uint8_t fails_low;
    uint8_t fails_high;
    uint8_t mode;
    uint8_t state;
};
void write_global_data_to_flash(void);
void read_global_data_from_flash(void);
#endif
