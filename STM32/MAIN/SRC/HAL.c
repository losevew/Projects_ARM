/*
 * HAL.c
 *
 *  Created on: 01.08.2010
 *      Author: Администратор
 */
#include "HAL.h"

void InitAll(void)
{
	RCC->APB2ENR|= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN;
	RCC->CFGR 	|= RCC_CFGR_MCO_SYSCLK;

	GPIOA->CRL	&= ~GPIO_CRL_CNF3;
	GPIOA->CRL 	|= GPIO_CRL_MODE3_0;

	GPIOB->CRL	&= ~GPIO_CRL_CNF5;
	GPIOB->CRL 	|= GPIO_CRL_MODE5_0;
	return;
}

