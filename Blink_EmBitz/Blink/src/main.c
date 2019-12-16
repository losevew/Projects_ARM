/*
**
**                           Main.c
**
**
**********************************************************************/
/*
   Last committed:     $Revision: 00 $
   Last changed by:    $Author: $
   Last changed date:  $Date:  $
   ID:                 $Id:  $

**********************************************************************/
#include "stm32f0xx.h"

int main(void)
{
    int i;

    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	GPIOA->MODER |= GPIO_MODER_MODER4_0;
    while(1)
      {
        //GPIOA->ODR |= GPIO_ODR_4;
            GPIOA->BSRR = GPIO_BSRR_BS_4;
            /* delay */
        for(i=0;i<0x250000;i++);

            //GPIOA->ODR &= ~GPIO_ODR_4;
            GPIOA->BSRR = GPIO_BSRR_BR_4;
            /* delay */
        for(i=0;i<0x100000;i++);
      }
}
