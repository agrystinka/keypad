#include "lcd_hd44780.h"
#include <stddef.h>

// Clear Display and Return Home commands
static const uint32_t DELAY_CLRRET_US = 1530;
// Read Data from RAM and Write Data to RAM commands
static const uint32_t DELAY_READWRITE_US = 43;
// Read Busy Flag and Address command
static const uint32_t DELAY_BUSYFLAG_US = 0;
// Entry Mode Set, Display ON/OFF Control, Cursor or Display Shift,
// Function Set, Set CGRAM Address, Set DDRAM Address commands
static const uint32_t DELAY_CONTROL_US = 39;
// LCD Enable (E) should be asserted for at least some time. Below is half period delay value
static const uint32_t DELAY_ENA_STROBE_US = 1;
// Initialization procedure have some specific delays. Here we have 2 delays for each init step
static const uint32_t DELAY_INIT0_US = 4100;
static const uint32_t DELAY_INIT1_US = 100;

void lcd_setup_default(void)
{
	gpio_mode_setup(mgl_get_gpio_port(sk_io_lcd_bkl.port), GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, mgl_get_gpio_pin(sk_io_lcd_bkl.pin));

	gpio_mode_setup(mgl_get_gpio_port(sk_io_lcd_rs.port), GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, mgl_get_gpio_pin(sk_io_lcd_rs.pin));

	gpio_mode_setup(mgl_get_gpio_port(sk_io_lcd_rw.port), GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, mgl_get_gpio_pin(sk_io_lcd_rw.pin));

	gpio_mode_setup(mgl_get_gpio_port(sk_io_lcd_en.port), GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, mgl_get_gpio_pin(sk_io_lcd_en.pin));

	gpio_mode_setup(mgl_get_gpio_port(sk_io_lcd_data.port), GPIO_MODE_OUTPUT,
				GPIO_PUPD_NONE, sk_io_lcd_data.pins);
}
/**
  * Private: Provides abstaction over two delay functions passed when constructing sk_lcd object
  *
  * Detect if optimal delay function is applicable and use it when possible with fallback
  * to unoptimal variants
  */
static void lcd_delay_us(struct sk_lcd *lcd, uint32_t us)
{
	if (NULL == lcd)
		return;

	sk_delay_func_t msfunc = lcd->delay_func_ms,
					usfunc = lcd->delay_func_us;

	if ((NULL == msfunc) && (NULL == usfunc))
		return;

	if (NULL == msfunc) {
		// only us-resolution func is set -> use unoptimal us delay
		usfunc(us);
		return;
	}

	if (NULL == usfunc) {
		// only ms-resolution func is set -> use rounded us delay
		msfunc((us % 1000) ? (1 + us / 1000) : (us / 1000));
		return;
	}

	// both functions are set -> use ms delay for divisor and us for remainder
	if (us / 1000)
		msfunc(us / 1000);
	if (us % 1000)
		usfunc(us % 1000);
}

void lcd_data_set_halfbyte(struct sk_lcd *lcd, uint8_t half)
{
	mgl_set_value(*lcd->pin_en, true);
	mgl_pin_group_set(*lcd->pin_group_data, half & 0x0F);
	lcd_delay_us(lcd, DELAY_ENA_STROBE_US);
	mgl_set_value(*lcd->pin_en, false);
	lcd_delay_us(lcd, DELAY_ENA_STROBE_US);
}

void lcd_data_set_byte(struct sk_lcd *lcd, uint8_t byte)
{
	if (lcd->is4bitinterface) {
		lcd_data_set_halfbyte(lcd, byte >> 4);
		lcd_data_set_halfbyte(lcd, byte & 0x0F);
	} else {
		// 8 bit data interface

		// (!) not implemented yet
	}
}

static inline sk_err lcd_rsrw_set(struct sk_lcd *lcd, bool rs, bool rw)
{
	mgl_set_value(*lcd->pin_rs, rs);
	mgl_set_value(*lcd->pin_rw, rw);
}

// Publically available low-level method. Issue low-level LCD command
sk_err _sk_lcd_cmd(struct sk_lcd *lcd, bool rs, bool rw, uint8_t *cmddata)
{
	if ((NULL == lcd) || (lcd->__isinitialized == false)) {
		// only initialized lcd should be used here
		// if not, at this level we don't what went wrong -> return unknown error
		return SK_EUNKNOWN;
	}

	if ((NULL == lcd->pin_rw) && rw) {
		// either we have rw pin controlled, or always grounded
		// in the latter case, trying to set it to 1 will obviously cause an error
		return SK_EUNAVAILABLE;
	}

	lcd_rsrw_set(lcd, rs, rw);
	lcd_data_set_byte(lcd, *cmddata);
	return SK_EOK;
}

// Basic command template to avoid redundant code
static inline sk_err _lcd_cmd_basic(struct sk_lcd *lcd, bool rs, bool rw, uint8_t data, uint32_t delay_us)
{
	sk_err err = _sk_lcd_cmd(lcd, rs, rw, &data);
	if (err != SK_EOK)
		return err;
	lcd_delay_us(lcd, delay_us);
	return SK_EOK;
}

// Clear Display command
sk_err sk_lcd_cmd_clear(struct sk_lcd *lcd)
{
	// data = 0b00000001
	return _lcd_cmd_basic(lcd, 0, 0, 0x01, DELAY_CLRRET_US);
}

// Return Home command
sk_err sk_lcd_cmd_rethome(struct sk_lcd *lcd)
{
	// data = 0b00000010
	return _lcd_cmd_basic(lcd, 0, 0, 0x02, DELAY_CLRRET_US);
}

// Entry Mode Set command
sk_err sk_lcd_cmd_emodeset(struct sk_lcd *lcd, bool isdirright, bool isshift)
{
	// data = 0b00000100 and
	// 	bit1 -- decrement/increment cnt (I/D),
	// 	bit0 -- display noshift / shift (SH)
	uint8_t data = 0x04 | (isdirright << 1) | (isshift << 0);
	return _lcd_cmd_basic(lcd, 0, 0, data, DELAY_CONTROL_US);
}

// Display ON/OFF Control command
sk_err sk_lcd_cmd_onoffctl(struct sk_lcd *lcd, bool display, bool cursor, bool blink)
{
	// data = 0b00001000 and
	// 	bit2 -- display on (D),
	// 	bit1 -- cursor on (C),
	// 	bit 0 -- blink on (B)
	uint8_t data = 0x08 | (display << 2) | (cursor << 1) | (blink << 0);
	return _lcd_cmd_basic(lcd, 0, 0, data, DELAY_CONTROL_US);
}

// Cursor or Display Shift command
sk_err sk_lcd_cmd_shift(struct sk_lcd *lcd, bool isshift, bool isdirright)
{
	// data = 0b00010000 and
	// 	bit3 -- move/shift display contents or cursor (S/C),
	// 	bit2 -- direction left/right (R/L)
	uint8_t data = 0x10 | (isshift << 3) | (isdirright << 2);
	return _lcd_cmd_basic(lcd, 0, 0, data, DELAY_CONTROL_US);
}

// Set CGRAM Address and Set DDRAM Address commands
sk_err sk_lcd_cmd_setaddr(struct sk_lcd *lcd, uint8_t addr, bool iscgram)
{
	// cgram has 6 bit and ddram has 7 bit addresses
	if ((addr & 0x80) || (iscgram && (addr & 0xC0)))
		return SK_EWRONGARG;

	// data = 0b01000000 -- cgram, 0b10000000 -- ddram and
	// bits 6..0 -- cgram addr
	// bits 7..0 -- dram addr
	uint8_t data = iscgram ? 0x40 : 0x80;
	data |= addr;
	return _lcd_cmd_basic(lcd, 0, 0, data, DELAY_CONTROL_US);
}

// Write data to CG or DDRAM
// TODO: change ret type
void sk_lcd_write_byte(struct sk_lcd *lcd, uint8_t byte)
{
	_sk_lcd_cmd(lcd, 1, 0, &byte);
	lcd_delay_us(lcd, DELAY_READWRITE_US);		// todo: refactor
}

// Map character and send to LCD at current cursor position
// TODO: change ret type
void sk_lcd_putchar(struct sk_lcd *lcd, const char ch)
{
	uint8_t conv = lcd->charmap_func(ch);
	sk_lcd_write_byte(lcd, conv);
}

void lcd_init_4bit(struct sk_lcd *lcd)
{
	lcd_rsrw_set(lcd, 0, 0);
	lcd_data_set_halfbyte(lcd, 0b0011);
	lcd_delay_us(lcd, DELAY_INIT0_US);

	// lcd_rsrw_set(lcd, 0, 0);
	lcd_data_set_halfbyte(lcd, 0b0010);
	lcd_delay_us(lcd, DELAY_INIT1_US);

	// lcd_rsrw_set(lcd, 0, 0);
	lcd_data_set_halfbyte(lcd, 0b0010);
	lcd_delay_us(lcd, DELAY_CONTROL_US);

	// set display on/off: display on (D), cursor off (C), blink off (B)
	sk_lcd_cmd_onoffctl(lcd, true, false, false);

	// clear display
	sk_lcd_cmd_clear(lcd);

	// entry mode set: increment cnt (I/D), noshift (SH)
	sk_lcd_cmd_emodeset(lcd, 1, 0);
}

// backlight control
sk_err sk_lcd_set_backlight(struct sk_lcd *lcd, uint8_t level)
{
	if (NULL == lcd)
		return SK_EWRONGARG;

	if (NULL != lcd->set_backlight_func) {
		// try to set with user provided function
		lcd->set_backlight_func(level);
	} else if (NULL != lcd->pin_bkl) {
		// fallback to direct pin control if available
		mgl_set_value(*lcd->pin_bkl, (bool)level);
	} else {
		return SK_EWRONGARG;
	}

	return SK_EOK;
}

// Initialize LCD
sk_err sk_lcd_init(struct sk_lcd *lcd)
{
	// check passed by pointer struct
	if (NULL == lcd)
		return SK_EWRONGARG;

	// check mandatory fields
	if ((NULL == lcd->pin_group_data) || (NULL == lcd->pin_rs) || (NULL == lcd->pin_en))
		return SK_ENENARG;

	// at least one of the functions must be used
	if ((NULL == lcd->delay_func_us) && (NULL == lcd->delay_func_ms))
		return SK_ENENARG;

	// *TODO* currently we only support 4-bit interface
	if (! lcd->is4bitinterface)
		return SK_ENOTIMPLEMENTED;

	// set default charmap function if not provided
	if (NULL == lcd->charmap_func)
		lcd->charmap_func = &sk_lcd_charmap_rus_cp1251;

	// set initialized bit so that methods checking it won't return error
	lcd->__isinitialized = true;

	lcd_init_4bit(lcd);
	return SK_EOK;
}

// don't map, return as-is for direct table use
uint8_t sk_lcd_charmap_none(const char c)
{
	return *((const uint8_t *)&c);
}

// CP1251 (aka Windows-1251) char map
uint8_t sk_lcd_charmap_rus_cp1251(const char c)
{
	uint8_t ch = *((uint8_t *)&c);		// treat char as byte
	if (ch < 128)
		return ch;

	switch (c) {
		case 'А': return 'A';
		case 'Б': return 0xA0;
		case 'В': return 'B';
		case 'Г': return 0xA1;
		case 'Ґ': return 0x04;		//
		case 'Д': return 0xE0;
		case 'Е': return 'E';
		case 'Ё': return 0xA2;
		case 'Ж': return 0xA3;
		case 'З': return 0xA4;
		case 'И': return 0xA5;
		case 'І':  return 'I';
		case 'Ї':  return 0x05;		//
		case 'Й': return 0xA6;
		case 'К': return 'K';
		case 'Л': return 0xA7;
		case 'М': return 'M';
		case 'Н': return 'H';
		case 'О': return 'O';
		case 'П': return 0xA8;
		case 'Р': return 'P';
		case 'С': return 'C';
		case 'Т': return 'T';
		case 'У': return 0xA9;
		case 'Ф': return 0xAA;
		case 'Х': return 'X';
		case 'Ц': return 0xE1;
		case 'Ч': return 0xAB;
		case 'Ш': return 0xAC;
		case 'Щ': return 0xE2;
		case 'Ъ': return 0xAD;
		case 'Ы': return 0xAE;
		case 'Ь': return 0x06;		//
		case 'Э': return 0xAF;
		case 'Є': return 0x03;		//
		case 'Ю': return 0xB0;
		case 'Я': return 0xB1;
		case 'а': return 'a';
		case 'б': return 0xB2;
		case 'в': return 0xB3;
		case 'г': return 0xB4;
		case 'ґ': return 0x00;		//
		case 'д': return 0xE3;
		case 'е': return 'e';
		case 'є': return 0x01;		//
		case 'ё': return 0xB5;
		case 'ж': return 0xB6;
		case 'з': return 0xB7;
		case 'и': return 0xB8;
		case 'і': return 'i';
		case 'ї': return 0x02;		//
		case 'й': return 0xB9;
		case 'к': return 0xBA;
		case 'л': return 0xBB;
		case 'м': return 0xBC;
		case 'н': return 0xBD;
		case 'о': return 'o';
		case 'п': return 0xBE;
		case 'р': return 'p';
		case 'с': return 'c';
		case 'т': return 0xBF;
		case 'у': return 'y';
		case 'ф': return 0xE4;
		case 'х': return 'x';
		case 'ц': return 0xE5;
		case 'ч': return 0xC0;
		case 'ш': return 0xC1;
		case 'щ': return 0xE6;
		case 'ъ': return 0xC2;
		case 'ы': return 0xC3;
		case 'ь': return 0xC4;
		case 'э': return 0xC5;
		case 'ю': return 0xC6;
		case 'я': return 0xC7;

		case '“': return 0xCA;
		case '”': return 0xCB;
		case '«': return 0xC8;
		case '»': return 0xC9;
		case '№': return 0xCC;
		case '°': return 0xEF;
		case '·': return 0xDF;

		default: return 0xFF;	// black square for unknown symbols
	}
}

void lcd_custom_char_load(struct sk_lcd *lcd)
{
	uint8_t custom_ch[8][8] = {                         //char   DDRAM address
		{0x00, 0x01, 0x1f, 0x10, 0x10, 0x10, 0x10, 0x00}, //'ґ'   0x00
		{0x00, 0x00, 0x0e, 0x10, 0x1c, 0x11, 0x0e, 0x00}, //'є'   0x01
		{0x00, 0x0a, 0x00, 0x04, 0x04, 0x04, 0x04, 0x00}, //'ї'   0x02
		{0x0e, 0x11, 0x10, 0x1c, 0x10, 0x11, 0x0e, 0x00}, //'Є'  0x03
		{0x01, 0x1f, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00}, //'Ґ'   0x04
		{0x0a, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x00}, //'Ї'   0x05
		{0x10, 0x10, 0x10, 0x1e, 0x11, 0x11, 0x1e, 0x00}, //'Ь'  0x06
		{0x00, 0x1b, 0x1f, 0x1f, 0x0e, 0x0e, 0x04, 0x00} //hurt emoji   0x07
	};
	sk_lcd_cmd_setaddr(lcd, 0x00, true);
	for(int i = 0; i < 8; i++)
		for(int j = 0; j < 8; j++)
			sk_lcd_write_byte(lcd, custom_ch[i][j]);

	sk_lcd_cmd_setaddr(lcd, 0x00, false);

}

//TODO: change chars
void lcd_custom_symb_load(struct sk_lcd *lcd)
{
	uint8_t custom_sb[8][8] = {                           //symbol    DDRAM address
		{0x04, 0x0e, 0x1f, 0x04, 0x04, 0x04, 0x04, 0x00}, //up        0x00
		{0x04, 0x04, 0x04, 0x04, 0x1f, 0x0e, 0x04, 0x00}, //down      0x01
		{0x00, 0x04, 0x06, 0x1f, 0x06, 0x04, 0x00, 0x00}, //right     0x02
		{0x00, 0x04, 0x0c, 0x1f, 0x0c, 0x04, 0x00, 0x00}, //left      0x03
		//{0x00, 0x00, 0x0e, 0x0e, 0x0e, 0x00, 0x00, 0x00}, //point     0x04
		{0x00, 0x0e, 0x1f, 0x1f, 0x1f, 0x0e, 0x00, 0x00}, //point     0x04
		{0x00, 0x0e, 0x11, 0x11, 0x1f, 0x1f, 0x1f, 0x00}, //locked    0x05
		{0x0e, 0x11, 0x11, 0x01, 0x1f, 0x1f, 0x1f, 0x00}, //unlocked  0x06
		{0x00, 0x11, 0x0a, 0x04, 0x0a, 0x11, 0x00, 0x00}  //close         0x07
	};

	sk_lcd_cmd_setaddr(lcd, 0x00, true);
	for(int i = 0; i < 8; i++)
		for(int j = 0; j < 8; j++)
			sk_lcd_write_byte(lcd, custom_sb[i][j]);

	sk_lcd_cmd_setaddr(lcd, 0x00, false);
}
