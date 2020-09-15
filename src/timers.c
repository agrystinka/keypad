#include "mygpiolib.h"
#include "timers.h"

//for realization of delay basedon SysTick
volatile uint32_t __nticks = 0;
const uint32_t TICK_RATE_HZ = 1000ul; //1kHz

/*
 * Realization of delay for N ms based on one systick.
 */
 //strong declaration of iterrupt
void sys_tick_handler(void)
{
	__nticks++;
}

void systick_setup(void)
{
	// disable just in case we're trying to reconfigure
	// while the SysTick is running
	systick_counter_disable();

	// frequency 16 MHz
	systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
	systick_interrupt_enable();

	// tick frequency will be 1 kHz (1 ms tick rate)
	uint32_t period = 16000000lu / 1000lu;
	systick_set_reload(period);
	// load value
	STK_CVR = period;

	nvic_set_priority(NVIC_SYSTICK_IRQ, 2);
	nvic_enable_irq(NVIC_SYSTICK_IRQ);
	__nticks = 0;

	systick_counter_enable();
}

void delay_ms_systick(uint32_t time_ms)
{
	uint32_t cur = __nticks;
	uint32_t delta = (TICK_RATE_HZ / 1000) * time_ms;
	uint32_t next = cur + delta;

	// overflow not taken into account here
	while (__nticks <= next) {
		__asm__ volatile ("wfi");
	}
}

/*
 * Realization of delay for N ms based on one shot timer.
 */
void one_shot_timer_tim7_setup(void)
{
	rcc_periph_clock_enable(RCC_TIM7);
	// 1 timer tick == 1 microsecond
	timer_set_prescaler(TIM7, 16 - 1);
	timer_disable_preload(TIM7);
	timer_one_shot_mode(TIM7);
	// Set URS bit to update only at overflow (when auto-reload value is reached)
	timer_update_on_overflow(TIM7);
	timer_enable_irq(TIM7, TIM_DIER_UIE);
	timer_clear_flag(TIM7, TIM_SR_UIF);		// just in case
	nvic_set_priority(NVIC_TIM7_IRQ, 2);
	nvic_enable_irq(NVIC_TIM7_IRQ);
}

static void one_shot_timer_tim7(uint16_t time_us)
{
	timer_set_period(TIM7, time_us);
    timer_generate_event(TIM7, TIM_EGR_UG);
	__dmb();
	timer_enable_counter(TIM7);
	__asm__ volatile ("wfi");
}

void delay_us_tim7(uint32_t time_us)
{
	while(time_us >= 1000){
		one_shot_timer_tim7(1000);
		time_us -= 1000;
	}
	if(time_us > 0)
		one_shot_timer_tim7(time_us);
}

//strong declaration of iterrupt
void tim7_isr(void)
{
	//for realization of delay for N us based on one shot timer.
	timer_clear_flag(TIM7, TIM_SR_UIF);
}
