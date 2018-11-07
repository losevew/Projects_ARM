/*
 * EERTOSHAL.c
 *
 *  Created on: 01.08.2010
 *      Author: DI HALT
 */
#include "EERTOSHAL.h"
#include "EERTOS.h"

//RTOS Запуск системного таймера
void RunRTOS (void)
{

 SysTick->LOAD=TimerTick;
 SysTick->VAL=TimerTick;
 SysTick->CTRL=(1<<st_ENABLE|1<<st_TICKINT|st_CPU<<st_CLKSOURCE);

 return;
}

//RTOS Interrupt
void SysTick_Handler(void)
{
TimerService();
}
