#ifndef TIMERS_H
#define TIMERS_H
#include <libopencm3/cm3/cortex.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/scb.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/sync.h>
#include <libopencm3/cm3/systick.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

/*
 * Realization of delay for N ms based on one sys tick.
 */
/**
 * systich_setup() - setup SysTick.
 *
 * Return: void.
 */
void systick_setup(void);
/**
 * delay_us_systick() - dalay based on systick.
 * @uint32_t time_ms: time in miliseconds.
 *
 * Return: void.
 */
void delay_ms_systick(uint32_t ms);
void sys_tick_handler(void);

/*
 * Realization of delay for N us based on one shot timer.
 */
 /**
  * one_shot_timer_tim7_setup() - setup TIM7 as one shot timer.
  *
  * Return: void.
  */
void one_shot_timer_tim7_setup(void);
/**
 * delay_us_tim7() - dalay based on one shot timer TIM7.
 * @uint32_t time_us: time in microseconds.
 *
 * Return: void.
 */
void delay_us_tim7(uint32_t time_us);
void tim7_isr(void);

//void pwm_timer_tim2_setup();
//void exti0_isr(void);
//void tim1_isr(void);
#endif
