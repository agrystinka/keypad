#include "menu.h"
#include <stdint.h>
#include <stdio.h>

uint8_t CHANGES = 0;

static void kp_set_min_fail_delay(struct sk_lcd *lcd)
{
    kp_screen_message(lcd, "Min fail delay", NULL);
    set_1_60(lcd, &FAIL_DELAY);
}

static void kp_set_fail_coef(struct sk_lcd *lcd)
{
    uint8_t coef[5] = {1, 2, 4, 8, 10};
    kp_screen_message(lcd, "Coefficient", NULL);
    scroll_num(lcd, &DELAY_COEFF, &coef[0], 5);
}

static void kp_set_fail_crit(struct sk_lcd *lcd)
{
    // sk_lcd_cmd_clear(lcd);
    // sk_lcd_cmd_setaddr(lcd, 0x00, false);
    // lcd_print(lcd, "Fail coefficient");
    kp_screen_message(lcd, "Critical", NULL);
    while(1){
        if(KP_CMD == KP_MENU){
            KP_CMD = KP_NONE;
            return;
        }
        KP_CMD = KP_NONE;
    }
}

static void kp_fail_settings(struct sk_lcd *lcd)
{
    //init Menu Data
    char menu_line0[] = " Go back";
    char menu_line1[] = " Minimal delay";
    char menu_line2[] = " Coefficient";
    char menu_line3[] = " Critical";
    char *menu_lines[] = {&menu_line0[0], &menu_line1[0], &menu_line2[0], &menu_line3[0]};

    void (*options[])(struct sk_lcd) = {NULL, kp_set_min_fail_delay, kp_set_fail_coef, kp_set_fail_crit};
    uint8_t num_lines = 4;

    struct menu fail_menu = {
        .num = num_lines,
        .lines = &menu_lines[0],
        .options = &options[0]
    };

    kp_menu_template(lcd, &fail_menu);
}

//single action
static void kp_mode1_settings(struct sk_lcd *lcd)
{
    sk_lcd_cmd_clear(lcd);
    sk_lcd_cmd_setaddr(lcd, 0x00, false);
    lcd_print(lcd, "Set delay:");
    set_1_60(lcd, &WELCOME_DELAY);
    //close door
    KP_MODE = true;
}

//change state
static void kp_mode2_settings(struct sk_lcd *lcd)
{
    //close door
    KP_MODE = false;
}

static void kp_mode_settings(struct sk_lcd *lcd)
{
    //init Menu Data
    char menu_line0[] = " Go back";
    char menu_line1[] = " Mode 1";
    char menu_line2[] = " Mode 2";
    char *menu_lines[] = {&menu_line0[0], &menu_line1[0], &menu_line2[0]};

    void (*options[])(struct sk_lcd) = {NULL, kp_mode1_settings, kp_mode2_settings};
    uint8_t num_lines = 3;

    struct menu mode_menu = {
        .num = num_lines,
        .lines = &menu_lines[0],
        .options = &options[0]
    };

    kp_menu_template(lcd, &mode_menu);
}

static void kp_main_change_pass(struct sk_lcd *lcd)
{
    // //ask for USER PASS
    // kp_input_password(lcd, USR_PASS_LENGTH, " Password");
    // if(!kp_check_plain(INPUT_PASS, USR_PASS, USR_PASS_LENGTH))
    //     return;
    //
    // //ask for MASTER_CODE
    // kp_input_password(lcd, MASTER_CODE_LENGTH, " Master code");
    // if(!kp_check_plain(INPUT_PASS, MASTER_CODE, MASTER_CODE_LENGTH))
    //     return;

    //set pass length (4...8)
    //input NEW
    uint8_t pass[MAX_PASS_LENGTH];
    uint8_t copy[MAX_PASS_LENGTH];
    kp_input_password(lcd, &pass[0], USR_PASS_LENGTH, " New password",  false);

    //input NEW once more
    kp_input_password(lcd, &copy[0], USR_PASS_LENGTH, " Repeat", false);

    if(kp_check_plain(&copy[0], &pass[0], copy)){
        for(int i = 0; i < USR_PASS_LENGTH; i++)
            USR_PASS[i] = pass[i];
        kp_screen_message(lcd, "  Password was", "    changed");
    }else{
        kp_screen_message(lcd, "  Error", NULL);
    }
    while (1){
        __asm__ volatile ("wfi");
        if(KP_CMD == KP_MENU)
            return;
    }
}

/**
 * kp_menu_template() - keypad main settings.
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
static void kp_main_settings(struct sk_lcd *lcd)
{
    //init Menu Data
    char menu_line0[] = " Go back";
    char menu_line1[] = " Password";
    char menu_line2[] = " Master code";
    char menu_line3[] = " Work mode";
    char menu_line4[] = " Fail";
    char *menu_lines[] = {&menu_line0[0], &menu_line1[0], &menu_line2[0], &menu_line3[0], &menu_line4[0]};
    void (*options[])(struct sk_lcd) = {NULL, kp_main_change_pass, NULL, NULL, kp_fail_settings};
    uint8_t num_lines = 5;

    struct menu main_menu = {
        .num = num_lines,
        .lines = &menu_lines[0],
        .options = &options[0]
    };

    kp_menu_template(lcd, &main_menu);
}

void kp_menu(struct sk_lcd *lcd)
{
    uint8_t pass[MAX_PASS_LENGTH];
    //check if user have permissions to use Settings
    kp_input_password(lcd, &pass[0], MASTER_CODE_LENGTH, " Master code", true);
    //check if correct MASTER_CODE
    if(kp_check_plain(&pass[0], MASTER_CODE, MASTER_CODE_LENGTH))
        kp_main_settings(lcd);
}
