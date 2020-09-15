#include "setup.h"

sk_err kp_lcd_init_setup(struct sk_lcd *lcd)
{
    rcc_periph_clock_enable(RCC_GPIOE);		// lcd is connected to port E
	lcd_setup_default();
	mgl_pin_group_set(sk_io_lcd_data, 0x00);
}

sk_err kp_btn_setup(void)
{
    rcc_periph_clock_enable(RCC_GPIOA); //bnt swt2
	rcc_periph_clock_enable(RCC_GPIOC); //btns swt1, swt3, swt4, swt5

    mgl_mode_setup_default(mgl_btn_swt1);
	mgl_mode_setup_default(mgl_btn_swt2);
	mgl_mode_setup_default(mgl_btn_swt3);
	mgl_mode_setup_default(mgl_btn_swt4);
	mgl_mode_setup_default(mgl_btn_swt5);
}

static sk_err kp_interrupts_btn_swt12_setup(void)
{
    // Set priority for EXTI15_10.
    const uint8_t group = 2;
    const uint8_t subgroup = 0;
    nvic_set_priority(NVIC_EXTI15_10_IRQ, (group << 2) | subgroup);
    //swt2
    exti_select_source(EXTI15, GPIOA);
    exti_set_trigger(EXTI15, EXTI_TRIGGER_FALLING);	// ACTIVE LOW, so trigger on 1->0 transition
    exti_enable_request(EXTI15);
    exti_reset_request(EXTI15);
    //swt1
    exti_select_source(EXTI11, GPIOC);
    exti_set_trigger(EXTI11, EXTI_TRIGGER_FALLING);	// ACTIVE LOW, so trigger on 1->0 transition
    exti_enable_request(EXTI11);
    exti_reset_request(EXTI11);

    nvic_enable_irq(NVIC_EXTI15_10_IRQ);
}

static sk_err kp_interrupts_btn_swt345_setup(void)
{
    // Set priority for EXTI9_5.
    const uint8_t group = 2;
    const uint8_t subgroup = 0;
    nvic_set_priority(NVIC_EXTI9_5_IRQ, (group << 2) | subgroup);

    //swt4
    exti_select_source(EXTI6, GPIOC);
    exti_set_trigger(EXTI6, EXTI_TRIGGER_FALLING);	// ACTIVE LOW, so trigger on 1->0 transition
    exti_enable_request(EXTI6);
    exti_reset_request(EXTI6);

    //swt5
    exti_select_source(EXTI8, GPIOC);
    exti_set_trigger(EXTI8, EXTI_TRIGGER_FALLING);	// ACTIVE LOW, so trigger on 1->0 transition
    exti_enable_request(EXTI8);
    exti_reset_request(EXTI8);

    //swt3
    exti_select_source(EXTI9, GPIOC);
    exti_set_trigger(EXTI9, EXTI_TRIGGER_FALLING);	// ACTIVE LOW, so trigger on 1->0 transition
    exti_enable_request(EXTI9);
    exti_reset_request(EXTI9);

    nvic_enable_irq(NVIC_EXTI9_5_IRQ);
}


sk_err kp_interrupts_btn_setup(void)
{
    // Set priority grouping. See p.228 Programming Manual for more
    scb_set_priority_grouping(SCB_AIRCR_PRIGROUP_GROUP4_SUB4);	// default
    // Enable EXTI port clock
    rcc_periph_clock_enable(RCC_SYSCFG);

    kp_interrupts_btn_swt12_setup();
    kp_interrupts_btn_swt345_setup();
}

// sk_err kp_flash_setup(void)
// {
//     rcc_periph_clock_enable(RCC_GPIOE);
// }
