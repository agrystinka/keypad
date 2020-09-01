#include "setup.h"

sk_err kp_lcd_setup(struct sk_lcd *lcd)
{
    rcc_periph_clock_enable(RCC_GPIOE);		// lcd is connected to port E
	lcd_setup_default();
	mgl_pin_group_set(sk_io_lcd_data, 0x00);

	systick_setup();
	cm_enable_interrupts();

	init_bkl_pwm();
	sk_lcd_init(&lcd);
	sk_lcd_cmd_onoffctl(&lcd, true, false, false); // display on, cursor off, blinking off
	sk_lcd_set_backlight(&lcd, 200);

	lcd_custom_symb_load(&lcd);
}

sk_err kp_btn_setup(void)
{
    rcc_periph_clock_enable(RCC_GPIOA); //bnt
	rcc_periph_clock_enable(RCC_GPIOC); //btn

	mgl_mode_setup_default(mgl_btn_swt1);
	mgl_mode_setup_default(mgl_btn_swt2);
	mgl_mode_setup_default(mgl_btn_swt3);
	mgl_mode_setup_default(mgl_btn_swt4);
	mgl_mode_setup_default(mgl_btn_swt5);
}

sk_err kp_interrupts_btn_setup(void)
{
    // Set priority grouping. See p.228 Programming Manual for more
    scb_set_priority_grouping(SCB_AIRCR_PRIGROUP_GROUP4_SUB4);	// default

    // Set priority for EXTI0.
    const uint8_t group = 2;
    const uint8_t subgroup = 0;
    nvic_set_priority(NVIC_EXTI15_10_IRQ, (group << 2) | subgroup);

    // Enable EXTI port clock
    rcc_periph_clock_enable(RCC_SYSCFG);

    exti_select_source(EXTI15, GPIOA);
    exti_set_trigger(EXTI15, EXTI_TRIGGER_FALLING);	// ACTIVE HIGH, so trigger on 0->1 transition
    exti_enable_request(EXTI15);
    exti_reset_request(EXTI15);
    nvic_enable_irq(NVIC_EXTI15_10_IRQ);
}
