#include "menu_template.h"
#include "keypad.h"
//#include "intrinsics.h"

/**
 * kp_show_active_line() - show point befor active line.
 * @struct sk_lcd *lcd: LCD for comunication with user.
 * @bool line1: true if first line is active, false if not.
 * @bool line2: true if second line is active, false if not.
 *
 * Return: void.
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

//TODO The same as print message
static void kp_menu_lines(struct sk_lcd *lcd, char* line0, char* line1)
{
    sk_lcd_cmd_clear(lcd);

    sk_lcd_cmd_setaddr(lcd, 0x00, false);
    if(line0 != NULL)
        lcd_print(lcd, line0);

    sk_lcd_cmd_setaddr(lcd, 0x40, false);
    if(line1 != NULL)
        lcd_print(lcd, line1);
}

void kp_menu_template(struct sk_lcd *lcd, struct menu *menu)
{
    uint8_t activeline = 0;
    uint8_t topline = 0;

    kp_menu_lines(lcd, menu->lines[topline], menu->lines[topline + 1]);
    kp_show_active_line(lcd, true, false);

    while(1){
        __WFI();
        //__asm__ volatile ("wfi");
        if(KP_CMD != KP_NONE){
            if(KP_CMD == KP_MENU){
                //discard command
                KP_CMD = KP_NONE;
                //GO BACK function is reservd as activeline == 0
                if(activeline == GO_BACK)
                     return;
                if(menu->options[activeline] != NULL)
                    (*(menu->options[activeline]))(lcd);
                //Go back to previous screen (menu)
                kp_menu_lines(lcd, menu->lines[topline], menu->lines[topline + 1]);
                kp_show_active_line(lcd, true, false);
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

void set_1_60(struct sk_lcd *lcd, uint8_t *num)
{
    if(*num > 60)
        *num = 60;
    else if(*num < 1)
        *num = 1;

    uint8_t number[2] = {*num % 100 / 10, *num % 10};
    uint8_t size = 2;
    uint8_t position = 0;

    sk_lcd_cmd_setaddr(lcd, 0x40 + 6, false);
    for(int i = 0; i < size; i++)
        lcd_print_int(lcd, number[i], 0);

    while(1){
        //sleep until user press on button
		__WFI();
        //__asm__ volatile ("wfi");
        if(KP_CMD != KP_NONE){
            if(KP_CMD == KP_MENU){
                *num = 0;
                for(int i = 0; i < size; i++)
                    *num = *num * 10 + number[i];
                KP_CMD = KP_NONE;
                return;
            }else if(KP_CMD == KP_UP){
                number[position] = (number[position] + 1) % 10;
            }else if(KP_CMD == KP_DOWN){
                number[position] = (number[position] - 1 + 10) % 10;
            }else if(KP_CMD == KP_LEFT && position > 0){
                position--;
            }else if(KP_CMD == KP_RIGHT && position < size - 1){
                position++;
            }
            if(number[0] * 10 + number[1] > 60)
                number[0] = 0;
            else if(number[0] * 10 + number[1] < 1)
                    number[1] = 1;
            sk_lcd_cmd_setaddr(lcd, 0x40 + 6 + position, false);
            lcd_print_int(lcd, number[position], 0);
            KP_CMD = KP_NONE;
        }
    }
}

void kp_scroll_num(struct sk_lcd *lcd, uint8_t *num, uint8_t *options, uint8_t size)
{
    uint8_t position = 0;
    *num = options[position];
    lcd_print_empty(lcd, 2);
    lcd_print_int(lcd, options[position], 0);
    while(1){
        // lcd_print_empty(lcd, 2);
        // lcd_print_int(lcd, options[position], 0);
        __WFI();
        //__asm__ volatile ("wfi");
        if(KP_CMD != KP_NONE){
            if(KP_CMD == KP_MENU){
                *num = options[position];
                KP_CMD = KP_NONE;
                return;
            }else if(KP_CMD == KP_UP){
                position = (position + 1) % size;
            }else if(KP_CMD == KP_DOWN){
                position = (position - 1 + size) % size;
            }
            lcd_print_empty(lcd, 2);
            lcd_print_int(lcd, options[position], 0);
            KP_CMD = KP_NONE;
        }
    }
}
