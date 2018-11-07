/*
 * EERTOS.h
 *
 *  Created on: 01.08.2010
 *      Author: Администратор
 */

#ifndef EERTOS_H_
#define EERTOS_H_

#include "HAL.h"
#include "EERTOSHAL.h"

void InitRTOS(void);
void Idle_task(void);

typedef void (*TPTR)(void);

void SetTask(TPTR TS);
void SetTimerTask(TPTR TS, uint32_t NewTime);

void TaskManager(void);
void TimerService(void);

//RTOS Errors Пока не используются.
#define TaskSetOk          'A'
#define TaskQueueOverflow    'B'
#define TimerUpdated       'C'
#define TimerSetOk          'D'
#define TimerOverflow       'E'

//BitBanding
#define IO_Zone 0x40000000
#define IO_Band 0x42000000


#define BBit(X,Y) ((X-IO_Zone)*0x20)+(Y*4)+IO_Band

#endif /* EERTOS_H_ */
