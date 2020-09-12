/**
 * my-gpio-lib - an abstraction over libopencm3/stm32/gpio
 * suted for Global Logic Started Kit
 */
#ifndef MYGPIOLIB_H
#define MYGPIOLIB_H
#include <libopencm3/stm32/gpio.h>
#include <stdint.h>
#include <stdbool.h>

//#define MGL_DEFAULT 0xff
/**
 * struct mgl_pin.
 * @gpio: Port from A to K.
 *        Size: 4 bits
 *        A = 0000, B = 0001, C = 0010, D = 0011, E = 0100,
 *        F = 0101, G = 0111, H = 1000, I = 1001, J = 1010,
 *        K = 1011, 1100 - 1111 are invalid codes
 * @pin: pin of port from 0 to 15
 *       Size: 4 bits
 * @reserved: reserved for future use
 *      Size: 7 bits
 *
 *
 * mgl_pin is a packed structure.
 */
 struct  __attribute__((packed, aligned(1))) mgl_pin {
     uint16_t port : 4;
     uint16_t pin : 4;
     uint16_t inverse : 1;
     uint16_t reserved : 7;
 };

 /**
  * struct mgl_pin_group.
  * @port: Port from A to K.
  *        Size: 4 bits
  *        A = 0000, B = 0001, C = 0010, D = 0011, E = 0100,
  *        F = 0101, G = 0111, H = 1000, I = 1001, J = 1010,
  *        K = 1011, 1100 - 1111 are invalid codes
  * @reserved: reserved for future use
  *            Size: 12 bits
  * @pins: each bit represents corresponding pin in a port
  *        Size: 16 bits
  * @inversions: each bit==1 represents inversion
  *              Size: 16 bits
  *
  * Represents group of pins residing in the same GPIO port.
  * Used as an optimization to allow reading/writing them all at once.
  * mgl_pin_group is a packed structure.
  */
struct __attribute__((packed, aligned(1))) mgl_pin_group {
	uint16_t port       : 4;
	uint16_t reserved : 12;
	uint16_t pins;
	uint16_t inversions;
};

typedef struct mgl_pin  mgl_pin;
typedef struct mgl_pin_group  mgl_pin_group;

 enum ports {
 	MGL_PORT_A = 0,
 	MGL_PORT_B = 1,
 	MGL_PORT_C = 2,
 	MGL_PORT_D = 3,
 	MGL_PORT_E = 4,
 	MGL_PORT_F = 5,
 	MGL_PORT_G = 6,
 	MGL_PORT_H = 7,
 	MGL_PORT_I = 8,
 	MGL_PORT_J = 9,
 	MGL_PORT_K = 10
 };

// some STM32F4DISCOVERY peripheral devices' pins
 extern const mgl_pin mgl_led_orange;
 extern const mgl_pin mgl_led_red;
 extern const mgl_pin mgl_led_green;
 extern const mgl_pin mgl_led_blue;
 extern const mgl_pin mgl_btn_usr;
// some GLSK peripheral devices' pins
 extern const mgl_pin mgl_btn_swt1;
 extern const mgl_pin mgl_btn_swt2;
 extern const mgl_pin mgl_btn_swt3;
 extern const mgl_pin mgl_btn_swt4 ;
 extern const mgl_pin mgl_btn_swt5;
 // GLSK LCD
 extern const mgl_pin sk_io_lcd_bkl;
 extern const mgl_pin sk_io_lcd_rs;
 extern const mgl_pin sk_io_lcd_rw;
 extern const mgl_pin sk_io_lcd_en;
 extern const mgl_pin_group sk_io_lcd_data;

 /**
  * An abstraction over basic GPIO functions.
  */
 void mgl_clear(mgl_pin peripheral_device);

 void mgl_set(mgl_pin peripheral_device);

 void mgl_toggle(mgl_pin peripheral_device);

 void mgl_set_value(mgl_pin periph, bool value);

 uint16_t mgl_port_read(mgl_pin periph);

 void mgl_port_write(mgl_pin periph, uint16_t data);

 void mgl_mode_setup_default(mgl_pin periph);

 void mgl_mode_setup(mgl_pin periph, uint8_t mode, uint8_t pull_up_down);

 uint32_t mgl_get_gpio_pin(uint8_t pin);
 uint32_t mgl_get_gpio_port(enum ports port);

/**
 * Functions for work with buttons.
 */
bool mgl_is_btn_pressed(mgl_pin btn);

/**
 * Functions for work with groups.
 */
void mgl_pin_group_set(mgl_pin_group group, uint16_t values);
#endif
