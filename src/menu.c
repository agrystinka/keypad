#include "menu.h"
#include "keypad.h"

uint8_t KP_MENU_ACTIVE = 0;
uint8_t activeline = 0;
uint8_t topline = 0;
uint8_t CHANGES = 0;



// const char menu_line1[] = "Go back";
// const char menu_line2[] = "Change deny delay";
// const char menu_line3[] = "Change welcome delay";
// const char menu_line4[] = "Change user password";

/* MENU scheme
 * Go back                   (0)
 * Change deny delay         (1)
 * Change welcome delay      (2)
 * Change user password      (3)
*/
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
// 
// kp_state kp_pass_input(struct sk_lcd *lcd, char* line0, char* line1)
// {
//     while(INPUT_PASS_LENGTH < PASS_LENGTH) {
//         if(KP_CMD != KP_NONE){
//             if(KP_CMD == KP_MENU){
//                 if(!KP_MENU_ACTIVE)
//                     kp_screen_menu(&lcd);
//                 else
//                 KP_CMD = KP_NONE;
//                 KP_MENU_ACTIVE = 1;
//                 INPUT_PASS_LENGTH = 0;//discard input password
//                 kp_screen_menu(&lcd);
//                 KP_MENU_ACTIVE = 0;
//             }
//             else {
//                 //disable exti btn interrupts
//                 INPUT_PASS[INPUT_PASS_LENGTH] = KP_CMD;
//                 INPUT_PASS_LENGTH++;
//                 kp_print_insecure(&lcd, KP_CMD);
//
//                 //if PASSWORD was written
//                 if(INPUT_PASS_LENGTH == PASS_LENGTH) {
//                     //block buttons
//                     kp_btn_disable();
//                     //check if password is correct
//                     if (kp_check_plain(&PASS[0], &INPUT_PASS[0], PASS_LENGTH)){ //open door
//                         return KP_ALLOW;
//                     }
//                     else{
//                         return KP_DENY;
//                     }
//                     INPUT_PASS_LENGTH = 0; //discard input password
//                     //enable interrupts
//                     kp_btn_enable();
//                     kp_screen_input(&lcd);
//                 }
//                 else if(INPUT_PASS_LENGTH > PASS_LENGTH){
//                     //an error occured, restart pasword input
//                     INPUT_PASS_LENGTH = 0; //discard input password
//                     kp_screen_input(&lcd);
//                 }
//             }
//
//             KP_CMD = KP_NONE;
//         }
//     }
// }

void kp_screen_menu(struct sk_lcd *lcd)
{
    char menu_line0[] = "  Go back";
    char menu_line1[] = "  Allow timer";
    char menu_line2[] = "  Deny timer";
    char menu_line3[] = "  User password";
    char *menu_lines[] = {&menu_line0[0], &menu_line1[0], &menu_line2[0], &menu_line3[0]};
    uint8_t num_lines = 4;

    uint8_t flag;

    kp_menu_lines(lcd, menu_lines[topline], menu_lines[topline + 1]);
    kp_show_active_line(lcd, true, false);

    while(KP_MENU_ACTIVE){
        if(KP_CMD != KP_NONE){
            if(KP_CMD == KP_MENU){
                if(activeline == WELCOME_TIMER){}
                else if(activeline == DENY_TIMER){}
                else if(activeline == CHANGE_PASS){}
                //if GO_BACK or an error occured
                else
                    KP_MENU_ACTIVE = 0;
                return;
            }
            else if(KP_CMD == KP_UP)
                activeline--;
            else  if(KP_CMD == KP_DOWN)
                activeline++;

            if(activeline < 0)
                activeline = 0;
            else if(activeline > num_lines - 1)
                activeline = num_lines - 1;

            if(activeline == topline){
                kp_show_active_line(lcd, true, false);
            }
            else if(activeline - topline == 1){
                kp_show_active_line(lcd, false, true);
            }
            else if(activeline - topline == 2){
                topline++;
                kp_menu_lines(lcd, menu_lines[topline], menu_lines[topline + 1]);
                kp_show_active_line(lcd, false, true);
            }
            else if(topline - activeline == 1){
                topline--;
                kp_menu_lines(lcd, menu_lines[topline], menu_lines[topline + 1]);
                kp_show_active_line(lcd, true, false);
            }
            KP_CMD = KP_NONE;
        }
    }
}
