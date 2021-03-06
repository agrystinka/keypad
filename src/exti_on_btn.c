#include "keypad.h"
#include "cmd.h"

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
	if(exti_get_flag_status(EXTI11)){
		if(sk_btn_debounce(mgl_btn_swt1)) //debouncing
			KP_CMD = KP_RIGHT;
		exti_reset_request(EXTI11);
	}else if(exti_get_flag_status(EXTI15)){
		if(sk_btn_debounce(mgl_btn_swt2)) //debouncing
			KP_CMD = KP_MENU;
		exti_reset_request(EXTI15);
	}
}


void exti9_5_isr(void)
{
	//check EXTI line
	if(exti_get_flag_status(EXTI6)){
		if(sk_btn_debounce(mgl_btn_swt4)) //debouncing
			KP_CMD = KP_UP;
		exti_reset_request(EXTI6);
	}else if(exti_get_flag_status(EXTI8)){
		if(sk_btn_debounce(mgl_btn_swt5)) //debouncing
			KP_CMD = KP_DOWN;
		exti_reset_request(EXTI8);
	}else if(exti_get_flag_status(EXTI9)){
		if(sk_btn_debounce(mgl_btn_swt3)) //debouncing
			KP_CMD = KP_LEFT;
		exti_reset_request(EXTI9);
	}
}
