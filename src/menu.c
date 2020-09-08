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
        if(KP_CMD != KP_NONE){
            if(KP_CMD == KP_MENU){
                if(activeline == 0) //Go back
                    return;
                (menu->options[activeline])();
                break;
            }
            else if(KP_CMD == KP_UP)
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

static void kp_set_min_fail_delay(struct sk_lcd *lcd)
{
    sk_lcd_cmd_clear(lcd);
    sk_lcd_cmd_setaddr(lcd, 0x00, false);
    lcd_print(lcd, "Min fail delay");
    //set number to variable
}

static void kp_set_fail_coef(struct sk_lcd *lcd)
{
    sk_lcd_cmd_clear(lcd);
    sk_lcd_cmd_setaddr(lcd, 0x00, false);
    lcd_print(lcd, "Fail coefficient");
    //set number to variable
}

static void kp_set_fail_crit(struct sk_lcd *lcd)
{
    sk_lcd_cmd_clear(lcd);
    sk_lcd_cmd_setaddr(lcd, 0x00, false);
    lcd_print(lcd, "Fail coefficient");
    //set number to variable
}

static void kp_go_back(struct sk_lcd *lcd)
{
    return;
}

static void kp_fail_settings(struct sk_lcd *lcd)
{
    //init Menu Data
    char menu_line0[] = " Go back";
    char menu_line1[] = " Minimal delay";
    char menu_line2[] = " Coefficient";
    char menu_line3[] = " Critical";
    char *menu_lines[] = {&menu_line0[0], &menu_line1[0], &menu_line2[0], &menu_line3[0]};

    void (*options[])(struct sk_lcd) = {kp_go_back, kp_set_min_fail_delay, kp_set_fail_coef, kp_set_fail_crit};
    uint8_t num_lines = 4;

    struct menu fail_menu = {
        .num = num_lines,
        .lines = &menu_lines[0],
        .options = &options[0]
    };

    kp_menu_template(lcd, &fail_menu);
}

static void kp_main_settings(struct sk_lcd *lcd)
{
    //init Menu Data
    char menu_line0[] = " Go back";
    char menu_line1[] = " Password";
    char menu_line2[] = " Master code";
    char menu_line3[] = " Welcome";
    char menu_line4[] = " Fail";
    char *menu_lines[] = {&menu_line0[0], &menu_line1[0], &menu_line2[0], &menu_line3[0], &menu_line4[0]};
    void (*options[])(struct sk_lcd) = {kp_go_back, kp_go_back, kp_go_back, kp_go_back, kp_fail_settings};
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
    kp_input_password(lcd, MASTER_CODE_LENGTH, " Master code");
    //check if correct MASTER_CODE
    if(kp_check_plain(INPUT_PASS, MASTER_CODE, MASTER_CODE_LENGTH)){
        //KP_MENU_ACTIVE = true;
        kp_main_settings(lcd);
        //KP_MENU_ACTIVE = false;
    }
    else{
        //access denied
        //wait 2 min untill next MASTER_CODE input will be allowed
        //go back to user pass input

    }
}
