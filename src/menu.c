#include "menu.h"
#include "keypad.h"

uint8_t CHANGES = 0;

static void kp_show_active_line(struct sk_lcd *lcd, bool line1, bool line2)
{
    sk_lcd_cmd_setaddr(lcd, 0x00, false);
    if(line1)
        lcd_print_symbol(lcd, POINT);
    else
        lcd_print(lcd, " ");

    sk_lcd_cmd_setaddr(lcd, 0x40, false);
    if(line2)
        lcd_print_symbol(lcd, POINT);
    else
        lcd_print(lcd, " ");
}

static void kp_menu_lines(struct sk_lcd *lcd, char* line0, char* line1)
{
    sk_lcd_cmd_clear(lcd);
    sk_lcd_cmd_setaddr(lcd, 0x00, false);
    lcd_print(lcd, line0);
    sk_lcd_cmd_setaddr(lcd, 0x40, false);
    lcd_print(lcd, line1);
}

static void kp_menu_template(struct sk_lcd *lcd, struct menu *menu)
{
    uint8_t activeline = 0;
    uint8_t topline = 0;

    kp_menu_lines(lcd, menu->lines[topline], menu->lines[topline + 1]);
    kp_show_active_line(lcd, true, false);

    while(1){
        __asm__ volatile ("wfi");
        if(KP_CMD != KP_NONE){
            if(KP_CMD == KP_MENU){
                //discard command
                KP_CMD = KP_NONE;
                //if GO_BACK or undefiened functions
                if(menu->options[activeline] == NULL)
                    return;
                (*(menu->options[activeline]))(lcd);
                    break;
            }
            else{
                if(KP_CMD == KP_UP)
                    activeline--;
                else  if(KP_CMD == KP_DOWN)
                    activeline++;

                //Barriers
                if(activeline < 0)
                    activeline = 0;
                else if(activeline > menu->num - 1)
                    activeline = menu->num - 1;

                //Refresh MENU screen if it is necessary
                if(activeline == topline){
                    kp_show_active_line(lcd, true, false);
                }
                else if(activeline - topline == 1){
                    kp_show_active_line(lcd, false, true);
                }
                else if(activeline - topline == 2){
                    topline++;
                    kp_menu_lines(lcd, menu->lines[topline], menu->lines[topline + 1]);
                    kp_show_active_line(lcd, false, true);
                }
                else if(topline - activeline == 1){
                    topline--;
                    kp_menu_lines(lcd, menu->lines[topline], menu->lines[topline + 1]);
                    kp_show_active_line(lcd, true, false);
                }
                //discard done command
                KP_CMD = KP_NONE;
            }
        }
    }
}

static void set_1_60(struct sk_lcd *lcd, uint8_t *num)
{
    uint8_t position = 10;
    uint8_t digit = 0;
    uint8_t mod = 6;

    if(*num > 60)
        *num = 60;
    else if(*num < 1)
        *num = 1;

    lcd_print_empty(lcd, 2);
    lcd_print(lcd, "\t\t\t");
    lcd_print_int(lcd, num, 0);

    while(1){
        __asm__ volatile ("wfi");
        if(KP_CMD != KP_NONE){
            if(KP_CMD == KP_MENU){
                KP_CMD = KP_NONE;
                return;
            }else if(KP_CMD == KP_UP){

                // digit = (digit + 1) % mod;
                // *num = *num / (position * 10) * (position * 10) + *num % position + digit * position + 1;
            }else if(KP_CMD == KP_DOWN){
                digit = (digit - 1 + mod) % mod;
                *num = *num / (position * 10) * (position * 10) + *num % position + digit * position + 1;
            }else if(KP_CMD == KP_LEFT && position == 1){
                position = 10;
                digit = *num / 10;
                mod = 6;
            }else if(KP_CMD == KP_RIGHT && position == 10){
                digit = *num % 10;
                position = 1;
                mod = 10;
            }
            lcd_print_empty(lcd, 2);
            lcd_print(lcd, "\t\t\t");
            lcd_print_int(lcd, *num, 0);
            KP_CMD = KP_NONE;
        }
    }
}

// static void set_1_60(struct sk_lcd *lcd, uint8_t *num)
// {
//     uint8_t number[2] = {0};
//     uint8_t size = 2;
//     uint8_t min = 1;
//     uint8_t max = 60;
//     uint8_t mod = 6;
//
//     if(*num < min)
//         *num = min;
//     else if(*num > max)
//         *num = max;
//
//     //put num to array
//     uint8_t position = size - 1;
//     uint8_t copy = *num;
//     while(position >= 0 || copy > 0){
//         number[position] = copy % 10;
//         copy /= 10;
//         position--;
//     }
//     position = 0;
//     sk_lcd_cmd_setaddr(lcd, 0x40 + 5, false);
//     for(int i = 0; i < size; i++)
//         lcd_print_int(lcd, number[i], 0);
//     while(1){
//         __asm__ volatile ("wfi");
//         if(KP_CMD != KP_NONE){
//             if(KP_CMD == KP_MENU){
//                 //array to number
//                 position = size - 1;
//                 *num = 0;
//                 while(position >= 0){
//                     *num = *num * 10 + number[position] + 1;
//                     position--;
//                 }
//                 KP_CMD = KP_NONE;
//                 return;
//             }else if(KP_CMD == KP_UP){
//                 number[position] = (number[position] + 1) % mod;
//                 // *num = *num / (position * 10) * (position * 10) + *num % position + digit * position + 1;
//             }else if(KP_CMD == KP_DOWN){
//                 number[position] = (number[position] - 1 + 10) % mod;
//                 //*num = *num / (position * 10) * (position * 10) + *num % position + digit * position + 1;
//             }else if(KP_CMD == KP_LEFT && position == 1){
//                 position = 0;
//                 mod = 6;
//             }else if(KP_CMD == KP_RIGHT && position == 0){
//                 position = 1;
//                 mod = 10;
//             }
//
//             sk_lcd_cmd_setaddr(lcd, 0x40 + 5 + position, false);
//             lcd_print_int(lcd, number[position], 0);
//             KP_CMD = KP_NONE;
//         }
//     }
// }

static void kp_set_min_fail_delay(struct sk_lcd *lcd)
{
    sk_lcd_cmd_clear(lcd);
    sk_lcd_cmd_setaddr(lcd, 0x00, false);
    lcd_print(lcd, "Min fail delay");

    set_1_60(lcd, &FAIL_DELAY);
}

static void kp_set_fail_coef(struct sk_lcd *lcd)
{
    sk_lcd_cmd_clear(lcd);
    sk_lcd_cmd_setaddr(lcd, 0x00, false);
    lcd_print(lcd, "Fail coefficient");

    while(1){
        if(KP_CMD == KP_MENU){
            KP_CMD = KP_NONE;
            return;
        }
        KP_CMD = KP_NONE;
    }
}

static void kp_set_fail_crit(struct sk_lcd *lcd)
{
    sk_lcd_cmd_clear(lcd);
    sk_lcd_cmd_setaddr(lcd, 0x00, false);
    lcd_print(lcd, "Fail coefficient");
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
    kp_input_password(lcd, USR_PASS_LENGTH, " New password");

    //copy to temp array
    uint8_t copy[MAX_PASS_LENGTH];
    for(int i = 0; i < USR_PASS_LENGTH; i++)
        copy[i] = INPUT_PASS[i];

    //input NEW once more
    kp_input_password(lcd, USR_PASS_LENGTH, " Repeat");

    if(kp_check_plain(copy, INPUT_PASS, copy)){
        for(int i = 0; i < USR_PASS_LENGTH; i++)
            USR_PASS[i] = INPUT_PASS[i];
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
    //check if user have permissions to use Settings
    kp_input_password(lcd, MASTER_CODE_LENGTH, " Master code");
    //check if correct MASTER_CODE
    if(kp_check_plain(INPUT_PASS, MASTER_CODE, MASTER_CODE_LENGTH))
        kp_main_settings(lcd);
}
