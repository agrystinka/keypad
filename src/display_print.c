#include "display.h"
#include <stdint.h>

void lcd_print_int(struct sk_lcd *lcd, int32_t num, char format)
{
	//hex
	if(format == 'h'){

	}
	//binary
	else if(format == 'b'){

	}
	//any other - decimal
	else{
		if(num == 0){
			sk_lcd_putchar(lcd, '0');
			return;
		}
		if (num < 0){
			sk_lcd_putchar(lcd, '-');
			num = 0 - num;
		}
		char str[16];
		int i = 0;
		while(num > 0){
			str[i] = num % 10 + '0';
			num /= 10;
			i++;
		}
		str[i] = '\0';

		i--;
		while(i >= 0){
			sk_lcd_putchar(lcd, str[i]);
			i--;
		}
	}
}

void lcd_print_n(struct sk_lcd *lcd)
{
	static uint8_t cnt = 0;
	if(cnt == 0){
		sk_lcd_cmd_setaddr(&lcd, 0x40, false);
	}
	else{
		//copy second line to first

		//clear second line
		//make delay for 1 s and clear display
		delay_ms_systick(1000);
		sk_lcd_cmd_clear(&lcd);
		//TODO: make one line up shift
	}
	cnt = 1 - cnt;
}

void lcd_print(struct sk_lcd *lcd, const char *format, ...)
{
	int cnt = 0;
	char *ptr = format;
	while (*ptr != '\0') {
		if(*ptr == '\n'){
			lcd_print_n(lcd);
	   	}
		else if(*ptr == '\t'){
			sk_lcd_putchar(lcd, ' ');
			sk_lcd_putchar(lcd, ' ');
	   	}
		else if(*ptr == '\r'){
			sk_lcd_cmd_rethome(lcd);
	   	}
		else{
			sk_lcd_putchar(lcd, *ptr);
		}
		ptr++;
		cnt++;
		if(cnt >= 16){
			cnt = 0;
			lcd_print_n(lcd);
		}
	}
}

void lcd_print_symbol(struct sk_lcd *lcd, uint8_t c)
{
	uint8_t address = 0;
	switch (c) {
		case UP:       address =  0x00; break;
		case DOWN:     address =  0x01; break;
		case RIGHT:    address =  0x02; break;
		case LEFT:     address =  0x03; break;
		case POINT:    address =  0x04; break;
		case LOCKED:   address =  0x05; break;
		case UNLOCKED: address =  0x06; break;
		case CLOSE:    address =  0x07; break;
		address =  0xFF;	 //black square for unknown symbols
	}
	sk_lcd_write_byte(lcd, address);
}

void lcd_print_time(struct sk_lcd *lcd, uint32_t time_s)
{
	//sk_lcd_cmd_setaddr(lcd, 0x40, false);
	//lcd_print(lcd, "\t\t\t");

	if(time_s / 60 == 0){
		sk_lcd_putchar(lcd, '0');
		sk_lcd_putchar(lcd, '0');
	} else if(time_s / 60 < 10) {
		sk_lcd_putchar(lcd, '0');
		lcd_print_int(lcd, time_s / 60, 0);
	} else {
		lcd_print_int(lcd, time_s / 60, 0);
	}

	sk_lcd_putchar(lcd, ':');

	if(time_s % 60 == 0){
		sk_lcd_putchar(lcd, '0');
		sk_lcd_putchar(lcd, '0');
	} else if(time_s % 60 < 10) {
		sk_lcd_putchar(lcd, '0');
		lcd_print_int(lcd, time_s % 60, 0);
	} else {
		lcd_print_int(lcd, time_s % 60, 0);
	}
}

void lcd_print_empty(struct sk_lcd *lcd, uint8_t line)
{
	if(line == 0){
		sk_lcd_cmd_clear(lcd);
	}else if(line == 1){
		sk_lcd_cmd_setaddr(lcd, 0x00, false);
		for(int i = 0; i < 16; i++)
			sk_lcd_putchar(lcd, ' ');
		sk_lcd_cmd_setaddr(lcd, 0x00, false);
	}else if(line == 2){
		sk_lcd_cmd_setaddr(lcd, 0x40, false);
		for(int i = 0; i < 16; i++)
			sk_lcd_putchar(lcd, ' ');
		sk_lcd_cmd_setaddr(lcd, 0x40, false);
	}
}
