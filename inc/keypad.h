#ifndef KP_KEYPAD_H
#define KP_KEYPAD_H

#include "cmd.h"
#include "setup.h"
#include "screen.h"
#include "menu.h"
#include "password.h"
#include "intrinsics.h"

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
//#include <stddef.h>

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

#endif
