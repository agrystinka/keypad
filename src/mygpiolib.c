#include "mygpiolib.h"
#include <libopencm3/stm32/gpio.h>
#include <stdint.h>
#include <stdbool.h>

//Definition of peripheral devices' pins

// some STM32F4DISCOVERY peripheral devices' pins
const mgl_pin mgl_led_orange = { .port=MGL_PORT_D, .pin=13, .inverse=0};
const mgl_pin mgl_led_red    = { .port=MGL_PORT_D, .pin=14, .inverse=0};
const mgl_pin mgl_led_green  = { .port=MGL_PORT_D, .pin=12, .inverse=0};
const mgl_pin mgl_led_blue   = { .port=MGL_PORT_D, .pin=15, .inverse=0};
const mgl_pin mgl_btn_usr    = { .port=MGL_PORT_A, .pin=0,  .inverse=0};
// some GLSK peripheral devices' pins
const mgl_pin mgl_btn_swt1   = { .port=MGL_PORT_C, .pin=11, .inverse=1};
const mgl_pin mgl_btn_swt2   = { .port=MGL_PORT_A, .pin=15, .inverse=1};
const mgl_pin mgl_btn_swt3   = { .port=MGL_PORT_C, .pin=9,  .inverse=1};
const mgl_pin mgl_btn_swt4   = { .port=MGL_PORT_C, .pin=6,  .inverse=1};
const mgl_pin mgl_btn_swt5   = { .port=MGL_PORT_C, .pin=8,  .inverse=1};
// GL-SK LCD 4-bit data interface pin group
const mgl_pin sk_io_lcd_bkl    = { .port=MGL_PORT_E, .pin=9,  .inverse=0};
const mgl_pin sk_io_lcd_rs     = { .port=MGL_PORT_E, .pin=7,  .inverse=0};
const mgl_pin sk_io_lcd_rw     = { .port=MGL_PORT_E, .pin=10, .inverse=0};
const mgl_pin sk_io_lcd_en     = { .port=MGL_PORT_E, .pin=11, .inverse=0};
const mgl_pin_group sk_io_lcd_data = {
	.port = MGL_PORT_E,
	.pins = (1 << 15) | (1 << 14) | (1 << 13) | (1 << 12),
	.inversions = 0
};

//Internal functions of mygpiolib-------------------------------------------
uint32_t mgl_get_gpio_pin(uint8_t pin)
{
    return 1 << pin;
}

uint32_t mgl_get_gpio_port(enum ports port)
{
    return GPIOA + (GPIOB - GPIOA) * port;
}

/**
 * An abstraction over basic GPIO functions.
 */

/**
 * mgl_clear() - set pin to 0.
 * @mgl_pin periph: peripheral device.
 *
 * Clear pin on port which are used by peripheral device.
 *
 * Return: void.
 */
void mgl_clear(mgl_pin periph)
{
    gpio_clear(mgl_get_gpio_port(periph.port), mgl_get_gpio_pin(periph.pin));
}

/**
 * mgl_set() - set pin to 1.
 * @mgl_pin periph: peripheral device.
 *
 * Set pin on port which are used by peripheral device.
 *
 * Return: void.
 */
void mgl_set(mgl_pin periph)
{
    gpio_set(mgl_get_gpio_port(periph.port), mgl_get_gpio_pin(periph.pin));
}

/**
 * mgl_toggle - invert pin.
 * @mgl_pin periph: peripheral device.
 *
 * Invert pin on port which are used by peripheral device.
 *
 * Return: void.
 */
void mgl_toggle(mgl_pin periph)
{
    gpio_toggle(mgl_get_gpio_port(periph.port), mgl_get_gpio_pin(periph.pin));
}

/**
 * mgl_set_value() - set pin to 1.
 * @mgl_pin periph: peripheral device.
 * @bool value: 0 or 1.
 *
 * Set specific value into pin on port which are used by peripheral device.
 *
 * Return: void.
 */
void mgl_set_value(mgl_pin periph, bool value)
{
    // if(value == true)
    //     gpio_set(mgl_get_gpio_port(periph.port), mgl_get_gpio_pin(periph.pin));
    // else if(value == false)
    //     gpio_clear(mgl_get_gpio_port(periph.port), mgl_get_gpio_pin(periph.pin));
	//
	if (value ^ periph.inverse)
		gpio_set(mgl_get_gpio_port(periph.port), mgl_get_gpio_pin(periph.pin));
	else
		gpio_clear(mgl_get_gpio_port(periph.port), mgl_get_gpio_pin(periph.pin));
}

/**
 * mgl_port_read() - read from peripheral device.
 * @mgl_pin periph: peripheral device.
 *
 * Be careful, because this function reads from port on which device work,
 * not on specific pin of this port.
 *
 * Return: uint16_t data.
 */
uint16_t mgl_port_read(mgl_pin periph)
{
    return gpio_port_read(mgl_get_gpio_port(periph.port));
}

/**
 * mgl_port_write() - write to peripheral device.
 * @mgl_pin periph: peripheral device.
 *
 * Be careful, because this function write to port on which device work,
 * not to specific pin of this port.
 *
 * Return: void.
 */
void mgl_port_write(mgl_pin periph, uint16_t data)
{
    gpio_port_write(mgl_get_gpio_port(periph.port), data);
}

/**
 * mgl_mode_setup_default() - setup device in default mode.
 * @mgl_pin periph: peripheral device.
 *
 * If device is LED, pin direction will be OUTPUT and output pin pullup will be NONE
 * If device is button, pin direction will be INPUT and output pin pullup will be PULLUP
 *
 * Return: void.
 */
void mgl_mode_setup_default(mgl_pin periph)
{
    uint8_t mode, pull_up_down;

    //if periph is a LED
    if((periph.port == mgl_led_orange.port && periph.pin == mgl_led_orange.pin) ||
       (periph.port == mgl_led_green.port && periph.pin == mgl_led_green.pin) ||
       (periph.port == mgl_led_red.port && periph.pin == mgl_led_red.pin) ||
       (periph.port == mgl_led_blue.port && periph.pin == mgl_led_blue.pin)) {
        mode = GPIO_MODE_OUTPUT;
        pull_up_down = GPIO_PUPD_NONE;
    }

    //if periph is a button on GLSK
    else
    if((periph.port == mgl_btn_swt1.port && periph.pin == mgl_btn_swt1.pin) ||
       (periph.port == mgl_btn_swt2.port && periph.pin == mgl_btn_swt2.pin) ||
       (periph.port == mgl_btn_swt3.port && periph.pin == mgl_btn_swt3.pin) ||
       (periph.port == mgl_btn_swt4.port && periph.pin == mgl_btn_swt4.pin) ||
       (periph.port == mgl_btn_swt5.port && periph.pin == mgl_btn_swt5.pin)) {
        mode = GPIO_MODE_INPUT;
        pull_up_down = GPIO_PUPD_PULLUP;
    }

    //if periph is STM_USR button
    else if(periph.port == mgl_btn_usr.port && periph.pin == mgl_btn_usr.pin){
        mode = GPIO_MODE_INPUT;
        pull_up_down = GPIO_PUPD_NONE;
    }
    else{
        mode = GPIO_MODE_OUTPUT;
        pull_up_down = GPIO_PUPD_NONE;
    }

    gpio_mode_setup(mgl_get_gpio_port(periph.port), mode, pull_up_down,
		mgl_get_gpio_pin(periph.pin));
}

/**
 * mgl_mode_setup() - setup device in default mode.
 * @mgl_pin periph: peripheral device.
 * @uint8_t mode: from 0 to 3 or GPIO constants:
 *                http://libopencm3.org/docs/latest/stm32f4/html/group__gpio__mode.html.
 * @uint8_t pull_up_down: from 0 to 2 or GPIO constants:
 *                        http://libopencm3.org/docs/latest/stm32f4/html/group__gpio__pup.html
 *
 * If you call this function with invalid arguments it will work as mgl_mode_setup_default()
 *
 * Return: void.
 */
void mgl_mode_setup(mgl_pin periph, uint8_t mode, uint8_t pull_up_down)
{
    //check for valid data
    //if invalid use default setup
    if(mode != GPIO_MODE_INPUT && mode != GPIO_MODE_OUTPUT &&
       mode != GPIO_MODE_AF && mode != GPIO_MODE_ANALOG)
        mgl_mode_setup_default(periph);
    else if(pull_up_down != GPIO_PUPD_NONE &&
            pull_up_down != GPIO_PUPD_PULLUP &&
            pull_up_down != GPIO_PUPD_PULLDOWN)
        mgl_mode_setup_default(periph);
    else
        gpio_mode_setup(mgl_get_gpio_port(periph.port), mode,
                        pull_up_down, mgl_get_gpio_pin(periph.pin));
}

/**
 * Functions for work with buttons.
 */

 /**
  * bool mgl_is_btn_pressed() - check if button is pressed.
  * @mgl_pin btn: button.
  *
  * Check if button is pressed.
  * Function contains simple button debouncing.
  * It is based on few times polling the port is there the ACTIVE signal.
  *
  * Return: bool.
  *         true - if button is pressed, fause- if button is not pressed.
  */
bool mgl_is_btn_pressed(mgl_pin btn)
{
    uint32_t btn_port = mgl_get_gpio_port(btn.port);
    uint32_t btn_pin = mgl_get_gpio_pin(btn.pin);

    //debouncing
    int cnt = 0;
    for(int i = 0; i < 10; i++)
        if(((gpio_port_read(btn_port) & btn_pin) == 0 && btn.inverse == 1) ||
           ((gpio_port_read(btn_port) & btn_pin) != 0 && btn.inverse == 0))
            cnt++;

    if(cnt >= 5){
        while(((gpio_port_read(btn_port) & btn_pin) == 0 && btn.inverse == 1) ||
           ((gpio_port_read(btn_port) & btn_pin) != 0 && btn.inverse == 0));
        return true;
    }
    else
        return false;
}

/*Work with group of pins*/
/**
 * Densification helper (private)
 * Example:
 *     mask = 0b1010000011110010
 *   sparse = 0b1010101010101010
 *   result = 0b1 1     1010  1
 */
static uint16_t group_densify(uint16_t mask, uint16_t sparse)
{
	uint16_t ret = 0;
	int idx = 0;
	for (int i = 0; i < 16; i++) {
		if (mask & (1 << i)) {
			ret |= sparse & (1 << i) ? (1 << idx) : 0;
			idx++;
		}
	}
	return ret;
}

/**
 * Sparsification helper (private)
 * Example:
 *     mask = 0b1010000011110010
 *    dense = 0b1 1     1010  1
 *   result = 0b1010000010100010
 */
static uint16_t group_sparsify(uint16_t mask, uint16_t dense)
{
	uint16_t ret = 0;
	int idx = 0;
	for (int i = 0; i < 16; i++) {
		if (mask & (1 << i)) {
			ret |= dense & (1 << idx) ? (1 << i) : 0;
			idx++;
		}
	}
	return ret;
}

void mgl_pin_group_set(mgl_pin_group group, uint16_t values)
{
	// We want to change only pins we use in group, and don't touch the others
	// We also want to manipulate the output register, not what's on port input at the moment,
    // so gpio_port_read() does not suit. Use GPIO ODR register directly
	values = group_sparsify(group.pins, values);
	values ^= group.inversions;
	volatile uint32_t *odr = &GPIO_ODR(mgl_get_gpio_port(group.port));
	uint32_t pval = *odr;
	pval &= ~((uint32_t)(group.pins));	// reset all pins in account
	pval |= values;			// set all pins in account to our values
	*odr = pval;
}
