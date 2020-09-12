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
//
// struct __attribute__((packed, aligned(1))) kp_lock {
//     uint8_t usrpass[MAX_PASS_LENGTH];
//     uint8_t mstrpass[MAX_PASS_LENGTH];
//     uint8_t menucode[MAX_PASS_LENGTH];
//     uint32_t delay_open_s;
//     uint32_t delay_wait_s;
//     uint32_t delay_wait_cur_s;
//     uint8_t fails;
//     uint16_t fails_low : 4;  //up to 16
//     uint16_t fails_high : 4; //up to 16
//     uint16_t usrpass_length : 3; //up to 8
//     uint16_t mstrpass_length : 3; //up to 8
//     uint16_t mode : 1; //bool
//     uint16_t state : 1; //bool
// };

// void pack_global_data(uint32_t *buffer);
// void unpack_global_data(uint32_t *buffer);
void pack_global_data(uint8_t *buffer);
void unpack_global_data(uint8_t *buffer);

void write_global_data_to_flash(void);
void read_global_data_from_flash(void);

void kp_welcome(struct sk_lcd *lcd, bool mode);
void kp_fail(struct sk_lcd *lcd);
void kp_btn_disable(void);
void kp_btn_enable(void);
#endif
