#include "keypad.h"

bool sk_btn_debounce(mgl_pin btn)
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

void exti15_10_isr(void)
{
	//check EXTI line
	if(exti_get_flag_status(EXTI15))
		if(sk_btn_debounce(mgl_btn_swt2)) //debouncing
			mgl_toggle(mgl_led_green);

	else if(exti_get_flag_status(EXTI11))
		if(sk_btn_debounce(mgl_btn_swt1)) //debouncing
			mgl_toggle(mgl_led_green);
}

void exti9_5_isr(void)
{
	//check EXTI line
	if(exti_get_flag_status(EXTI6))
		if(sk_btn_debounce(mgl_btn_swt4)) //debouncing
			mgl_toggle(mgl_led_blue);

	else if(exti_get_flag_status(EXTI8))
		if(sk_btn_debounce(mgl_btn_swt5)) //debouncing
			mgl_toggle(mgl_led_blue);

	else if(exti_get_flag_status(EXTI9))
		if(sk_btn_debounce(mgl_btn_swt3)) //debouncing
			mgl_toggle(mgl_led_blue);
}
