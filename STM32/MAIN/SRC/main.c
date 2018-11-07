/*
===============================================================================
 Name        : main.c
 Author      : DI HALT
 Version     :
 Copyright   : (C) Copyright
 Description : main definition
===============================================================================
*/

#include "HAL.h"
#include "EERTOS.h"

// Прототипы задач ============================================================
void Task1 (void);
void Task2 (void);
void TimerT (void);


// Задачи
void Task1 (void)
{
	GPIOA->BSRR =GPIO_BSRR_BS3;
	GPIOB->BSRR =GPIO_BSRR_BS5;
	SetTimerTask(Task2,50);
	return;
}

void Task2 (void)
{
	GPIOA->BSRR =GPIO_BSRR_BR3;
	GPIOB->BSRR =GPIO_BSRR_BR5;
	SetTimerTask(Task1,10);
	return;
}

void TimerT (void)
{
	SetTimerTask(TimerT,100);
	return;
}



int main(void)
{
//	SystemInit();
	InitAll();         // Инициализируем периферию
	InitRTOS();         // Инициализируем ядро
	RunRTOS();         // Старт ядра.

	SetTask(Task1);
	SetTask(TimerT);

	while(1)       // Главный цикл диспетчера
	{
	TaskManager();   // Вызов диспетчера
	}
}

