#include "menu.h"
#include "keypad.h"
#include "menu_template.h"
#include "password.h"
#include <stdint.h>
#include <stdio.h>

/** Used as flag to make system known if it is necessary
* to rewrite non-volitile memory with new SETTING DATA*/
bool CHANGES = false;

//Failures settings (Security settings)-----------------------------------------
static void kp_set_min_fail_delay(struct sk_lcd *lcd, struct kp_lock *keypad)
{
    uint32_t copy = keypad->delay_wait_s;

    kp_screen_message(lcd, "Min fail delay", NULL);
    set_1_60(lcd, &(keypad->delay_wait_s));
    keypad->delay_wait_cur_s = keypad->delay_wait_s;

    if(copy != keypad->delay_wait_s)
        CHANGES = true;
}

static void kp_set_fail_coef(struct sk_lcd *lcd, struct kp_lock *keypad)
{
    uint8_t copy = keypad->wait_coef;

    uint8_t coef[5] = {1, 2, 4, 8, 10};
    kp_screen_message(lcd, "Coefficient", NULL);
    kp_scroll_num(lcd, &(keypad->wait_coef), &coef[0], 5);

    if(copy != keypad->wait_coef)
        CHANGES = true;
}

static void kp_set_fail_crit_high(struct sk_lcd *lcd, struct kp_lock *keypad)
{
    uint8_t copy = keypad->fails_high;

    uint8_t coef[3] = {3, 5, 10};
    kp_screen_message(lcd, "Critical high", NULL);
    kp_scroll_num(lcd, &(keypad->fails_high), &coef[0], 3);

    if(copy != keypad->fails_high)
        CHANGES = true;
}

static void kp_set_fail_crit_low(struct sk_lcd *lcd, struct kp_lock *keypad)
{
    uint8_t copy = keypad->fails_low;

    uint8_t coef[3] = {1, 3, 5};
    kp_screen_message(lcd, "Critical low", NULL);
    kp_scroll_num(lcd, &(keypad->fails_low), &coef[0], 3);

    if(copy != keypad->fails_low)
        CHANGES = true;
}

static void kp_fail_settings(struct sk_lcd *lcd, struct kp_lock *keypad)
{
    //init Fail Menu Data
    char menu_line0[] = " Go back (Save)";
    char menu_line1[] = " Minimal delay";
    char menu_line2[] = " Coefficient";
    char menu_line3[] = " Critical high";
    char menu_line4[] = " Critical low";
    char *menu_lines[] = {&menu_line0[0], &menu_line1[0], &menu_line2[0], &menu_line3[0], &menu_line4[0]};

    void (*options[])(struct sk_lcd*, struct kp_lock*) = {NULL, kp_set_min_fail_delay, kp_set_fail_coef, kp_set_fail_crit_high, kp_set_fail_crit_low};
    uint8_t num_lines = 5;

    struct menu fail_menu = {
        .num = num_lines,
        .lines = &menu_lines[0],
        .options = &options[0]
    };

    kp_menu_template(lcd, keypad, &fail_menu);
}

//Keypad work mode settings-----------------------------------------------------
//single action
static void kp_mode1_settings(struct sk_lcd *lcd, struct kp_lock *keypad)
{
    uint32_t copy = keypad->delay_open_s;
    kp_screen_message(lcd, "Welcome delay", NULL);
    set_1_60(lcd, &(keypad->delay_open_s));
    //lock keypad - just in case
    keypad->mode = true;

    if(copy != keypad->delay_open_s)
        CHANGES = true;
}

//change state
static void kp_mode2_settings(struct sk_lcd *lcd, struct kp_lock *keypad)
{
    //lock keypad - just in case
    keypad->mode = false;
}

static void kp_mode_settings(struct sk_lcd *lcd, struct kp_lock *keypad)
{
    //init Menu Data
    char menu_line0[] = " Go back (Save)";
    char menu_line1[] = " Mode 1";
    char menu_line2[] = " Mode 2";
    char *menu_lines[] = {&menu_line0[0], &menu_line1[0], &menu_line2[0]};

    void (*options[])(struct sk_lcd*, struct kp_lock*) = {NULL, kp_mode1_settings, kp_mode2_settings};
    uint8_t num_lines = 3;

    struct menu mode_menu = {
        .num = num_lines,
        .lines = &menu_lines[0],
        .options = &options[0]
    };

    bool copy = keypad->mode;
    kp_menu_template(lcd, keypad, &mode_menu);
    if(copy != keypad->mode)
        CHANGES = true;
}
//Password and Master code settings---------------------------------------------
static void kp_set_length(struct sk_lcd *lcd, uint8_t *length, char *instruction)
{
    uint8_t coef[3] = {4, 6, 8};
    kp_screen_message(lcd, instruction, NULL);
    kp_scroll_num(lcd, length, &coef[0], 3);
}

static void kp_main_change_pass(struct sk_lcd *lcd, struct kp_lock *keypad, uint8_t *pass, uint8_t *passlength)
{
    //for bufferization input new password
    uint8_t length = 0;
    uint8_t buf1[MAX_PASS_LENGTH];
    uint8_t buf2[MAX_PASS_LENGTH];

    //in order to make system more secure
    //it was decided to ask USER PASSWORD and/or MASTER CODE one more time

    //ask for USER PASS
    // kp_input_password(lcd, &buf1[0], keypad->usrpass_length, " User pass", true);
    // if(!kp_check_plain(&buf1[0], keypad->usrpass, keypad->usrpass_length))
    //     return;

    //ask for MASTER_CODE
    // kp_input_password(lcd, &buf1[0], keypad->mstrpass_length, " Master pass", true);
    // if(!kp_check_plain(&buf1[0], keypad->mstrpass, keypad->mstrpass_length))
    //     return;

    kp_set_length(lcd, &length, "Set length");
    //input NEW password
    kp_input_password(lcd, &buf1[0], length, "New pass:",  false);
    //input NEW password once more
    kp_input_password(lcd, &buf2[0], length, " Repeat", false);
    //check if input passwords are equal
    if(kp_check_plain(&buf1[0], &buf2[0], length)){
        //change global variables
        *passlength = length;
        for(int i = 0; i < length; i++)
            *(pass + i) = buf1[i];
        //Message to user
        kp_screen_message(lcd, "Success. Pass", "was changed");
        CHANGES = true;
    }else{
        //Message to user
        kp_screen_message(lcd, "Error. Pass", "was not changed");
    }
    //Wait untill user press MENU button
    // while (1){
    //     __asm__ volatile ("wfi");
    //     if(KP_CMD == KP_MENU)
    //         return;
    // }
}

static void kp_main_change_usr_pass(struct sk_lcd *lcd, struct kp_lock *keypad)
{
    kp_main_change_pass(lcd, keypad, &(keypad->usrpass), &(keypad->usrpass_length));
}

static void kp_main_change_master_code(struct sk_lcd *lcd, struct kp_lock *keypad)
{
    kp_main_change_pass(lcd, keypad, &(keypad->mstrpass), &(keypad->mstrpass_length));
}

//Main settings-----------------------------------------------------------------
/**
 * kp_main_settings() - keypad main settings.
 *
 * Main setting contains such options:
 * Go back - exit from settings
 * Password - change user password
 * Master code - change umaster code
 * Work mode - change keypad work mode
 * Fail - change security settings
 *
 * Return: void.
 */
static void kp_main_settings(struct sk_lcd *lcd, struct kp_lock *keypad)
{
    //init Menu Data
    char menu_line0[] = " Go back (Save)";
    char menu_line1[] = " Password";
    char menu_line2[] = " Master code";
    char menu_line3[] = " Work mode";
    char menu_line4[] = " Fail";
    char *menu_lines[] = {&menu_line0[0], &menu_line1[0], &menu_line2[0], &menu_line3[0], &menu_line4[0]};
    void (*options[])(struct sk_lcd*, struct kp_lock*) = {NULL, kp_main_change_usr_pass, kp_main_change_master_code, kp_mode_settings, kp_fail_settings};
    uint8_t num_lines = 5;

    struct menu main_menu = {
        .num = num_lines,
        .lines = &menu_lines[0],
        .options = &options[0]
    };
    kp_menu_template(lcd, keypad, &main_menu);
}

void kp_menu(struct sk_lcd *lcd, struct kp_lock *keypad)
{
    uint8_t pass[MAX_PASS_LENGTH];
    //check if user have permissions to use Settings
    kp_input_password(lcd, &pass[0], keypad->mstrpass_length, " Master pass", true);
    //check if correct MASTER_CODE
    if(kp_check_plain(&pass[0], keypad->mstrpass, keypad->mstrpass_length)){
        kp_main_settings(lcd, keypad);
        //show wait screen, switch orange diod
        //rewrite non-volitile embeded flash memory if it is necessary
        if(CHANGES){
            kp_btn_disable();
            write_global_data_to_flash(keypad);
            kp_btn_enable();
        }
        //refresh security data after settings
        keypad->fails = keypad->fails_low;
        keypad->delay_wait_cur_s = keypad->delay_wait_s;
    }
}
