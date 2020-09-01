#include "display.h"
#include <libopencm3/cm3/cortex.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/scb.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>

void init_bkl_pwm(void)		// dirty code, needs refactoring
{
	nvic_set_priority(NVIC_EXTI0_IRQ, 2 << 2 | 3);
	rcc_periph_clock_enable(RCC_GPIOA);
	gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO0);		// pulldown is external
	rcc_periph_clock_enable(RCC_SYSCFG);
	exti_select_source(EXTI0, GPIOA);
	exti_set_trigger(EXTI0, EXTI_TRIGGER_RISING);
	exti_enable_request(EXTI0);
	exti_reset_request(EXTI0);
	nvic_enable_irq(NVIC_EXTI0_IRQ);

	gpio_set_output_options(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, 1 << 9);
	gpio_set_af(GPIOE, GPIO_AF1, 1 << 9);
	gpio_mode_setup(GPIOE, GPIO_MODE_AF, GPIO_PUPD_NONE, 1 << 9);

	rcc_periph_clock_enable(RCC_TIM1);

	timer_set_mode(TIM1, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
	timer_set_prescaler(TIM1, 16000000 / (256*1000));
	timer_enable_preload(TIM1);
	timer_set_period(TIM1, 255);
	//timer_continuous_mode(TIM1);
	timer_set_oc_mode(TIM1, TIM_OC1, TIM_OCM_PWM1);
	timer_enable_oc_output(TIM1, TIM_OC1);
	timer_enable_break_main_output(TIM1);
	timer_set_oc_value(TIM1, TIM_OC1, 0);
	timer_enable_counter(TIM1);
}

void timer1_set_pwm_backlight(uint8_t val)
{
	timer_set_oc_value(TIM1, TIM_OC1, val);	// we have TIM1_CH1 connected to backlight
}

void exti0_isr(void)		// dirty code, needs refactoring
{
	static uint8_t bkl = 0;
	sk_lcd_set_backlight(&lcd, bkl-1u);
	bkl += 16u;
	exti_reset_request(EXTI0);
}
