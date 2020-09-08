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
void kp_num_set(struct sk_lcd *lcd, uint8_t *num, uint8_t min, uint8_t max)
{
    if(*num < min)
        *num = min;
    else if(*num > max)
        *num = max;

    //calculate weight of max num
    uint8_t weight = 1;
    while(max / 10 / weight > 0)
        weight *= 10;
    uint8_t i = 1;
    uint8_t digit = 0;
    do{
        lcd_print_empty(lcd, 2);
        lcd_print(lcd, "\t\t\t");
        lcd_print_int(lcd, num, 0);
        //sleep
        if(KP_CMD == KP_MENU){
            KP_CMD = KP_NONE;
            return;
        }
        else if(KP_CMD == KP_UP)
            digit = (digit + 1) % 10;
        else  if(KP_CMD == KP_DOWN)
            digit = (digit - 1 + 10) % 10;
        else if(KP_CMD == KP_RIGHT){
            i /= 10;
            if(i < 1)
                i = 1;
            else
                digit = *num % (i * 10) / i;
        }
        else  if(KP_CMD == KP_LEFT){
            i *= 10;
            if(i > weight)
                i = weight;
            else
                digit = *num % (i * 10) / i;
        }
        *num  = *num / (i * 10) * (i * 10) + *num % i + i * digit;
    }while(1);
}
// void kp_num_scroll(struct sk_lcd *lcd, uint8_t data* uint8_t num)
// {
//     if(KP_CMD == KP_MENU){
//         //discard command
//         KP_CMD = KP_NONE;
//         //if GO_BACK or undefiened functions
//         if(menu->options[activeline] == NULL)
//             return;
//         (*(menu->options[activeline]))(lcd);
//         break;
//     }
//     else if(KP_CMD == KP_UP)
//         activeline--;
//     else  if(KP_CMD == KP_DOWN)
//         activeline++;
// }

static void kp_set_min_fail_delay(struct sk_lcd *lcd)
{
    sk_lcd_cmd_clear(lcd);
    sk_lcd_cmd_setaddr(lcd, 0x00, false);
    lcd_print(lcd, "Min fail delay");

    //FAIL_DELAY = kp_num_scroll(lcd, FAIL_DELAY, 0xFFFFFFFF);
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
    //Set delay
    KP_MODE = true;
}

//change state
static void kp_mode2_settings(struct sk_lcd *lcd)
{
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

static void kp_main_settings(struct sk_lcd *lcd)
{
    //init Menu Data
    char menu_line0[] = " Go back";
    char menu_line1[] = " Password";
    char menu_line2[] = " Master code";
    char menu_line3[] = " Work mode";
    char menu_line4[] = " Fail";
    char *menu_lines[] = {&menu_line0[0], &menu_line1[0], &menu_line2[0], &menu_line3[0], &menu_line4[0]};
    void (*options[])(struct sk_lcd) = {NULL, NULL, NULL, NULL, kp_fail_settings};
    uint8_t num_lines = 5;

    struct menu main_menu = {
        .num = num_lines,
        .lines = &menu_lines[0],
        .options = &options[0]
    };

    kp_menu_template(lcd, &main_menu);
}


static void kp_main_change_pass(struct sk_lcd *lcd)
{
    kp_input_password(lcd, USR_PASS_LENGTH, " Password");
    //check if correct MASTER_CODE
    if(!kp_check_plain(INPUT_PASS, USR_PASS, USR_PASS_LENGTH))
        return;

    kp_input_password(lcd, MASTER_CODE_LENGTH, " Master code");
    //check if correct MASTER_CODE
    if(!kp_check_plain(INPUT_PASS, MASTER_CODE, MASTER_CODE_LENGTH))
        return;
    //set pass length (4...8)
    //input new PASS 1
    //input new PASS 2
    //check if thaey are equal
    //if no Error message, go to main MENU
    //if yes OK message, go to main MENU
}

void kp_menu(struct sk_lcd *lcd)
{
    //check if user have permissions to use Settings
    kp_input_password(lcd, MASTER_CODE_LENGTH, " Master code");
    //check if correct MASTER_CODE
    if(kp_check_plain(INPUT_PASS, MASTER_CODE, MASTER_CODE_LENGTH))
        kp_main_settings(lcd);
}
