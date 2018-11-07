/*
 * EERTOSHAL.h
 *
 *  Created on: 01.08.2010
 *      Author: Администратор
 */

#ifndef EERTOSHAL_H_
#define EERTOSHAL_H_

#include "HAL.h"

//RTOS Config
#define TaskQueueSize      	30
#define MainTimerQueueSize  30
#define TimerTick  			F_CPU/1000

//SysTimerDefine
#define st_ENABLE 		0
#define st_TICKINT 		1
#define st_CLKSOURCE	2
#define st_COUNTFLAG	16
#define	st_CPU			1
#define	st_DIV			0

void RunRTOS (void);
void SysTick_Handler(void);

#endif /* EERTOSHAL_H_ */
