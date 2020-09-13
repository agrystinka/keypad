#ifndef KP_KEYPAD_H
#define KP_KEYPAD_H

#include "intrinsics.h"
#include "mygpiolib.h"
#include "timers.h"
#include "display.h"
#include "errors.h"
#include "lcd_hd44780.h"
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

// Recomended variants of password length
#define MAX_PASS_LENGTH   8
#define MIDLE_PASS_LENGTH 6
#define MIN_PASS_LENGTH   4

extern uint8_t STATE_SYMBOL;
extern uint8_t KP_CMD;

/*Weight of kp_lock structure is:
 * 3 pass * 8 chars * 1 byte = 24 bytes
 * 3 delays * 4 bytes = 12 bytes
 * 6 variables * 1 byte = 6 bytes
 * 2 bool variables * (in worse case) 4 bytes = 8bytes
 * Total: 50 bytes */
struct __attribute__((packed, aligned(1))) kp_lock {
    /** User Password to open/close keypad lock*/
    uint8_t usrpass[MAX_PASS_LENGTH];
    /** Master Password to change settings*/
    uint8_t mstrpass[MAX_PASS_LENGTH];
    /** Code to open settings*/
    uint8_t menucode[MAX_PASS_LENGTH];
    /** Time during which keypad will be unlocked after input of correct User Pass
     * (if keypad words in Mode 1)*/
    uint32_t delay_open_s;
    /** Time during which keypad will be blocked after input of incorrect User Pass
     * fails_low times.*/
    uint32_t delay_wait_s;
    /** Time during which keypad will be blocked after input of incorrect User Pass
     * more then fails_low times.*/
    uint32_t delay_wait_cur_s;
    /** Number of inputs of incorrect User Pass*/
    uint8_t fails;
    /** Number of inputs of incorrect User Pass after wich keypad lock blocks on delay_wait_cur_s*/
    uint8_t fails_low;  //up to 16
    /** Number of inputs of incorrect User Pass after wich keypad lock asks Master Pass*/
    uint8_t fails_high; //up to 16
    /** Growth multiplier of delay_wait_cur_s time after input of incorrect User Pass*/
    uint8_t wait_coef;
    /** Length of User Password and Menu code*/
    uint8_t usrpass_length; //up to 8
    /** Length of Master Password*/
    uint8_t mstrpass_length; //up to 8
    /** Keypad mode: if false - keypad change state after input corect User Pass,
     * if true - open (unlock) keypad for delay_open_s time*/
    bool mode;
    /** Keypad state: if false - closed (locked), if true - opened (unlocked)*/
    bool state;
};


void write_keypad_data_to_flash(struct kp_lock *keypad);
void read_keypad_data_from_flash(struct kp_lock *keypad);

void kp_welcome(struct sk_lcd *lcd, struct kp_lock *keypad);
void kp_fail(struct sk_lcd *lcd, struct kp_lock *keypad);
void kp_btn_disable(void);
void kp_btn_enable(void);
#endif
