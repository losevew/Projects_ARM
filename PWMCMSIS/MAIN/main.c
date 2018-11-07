#include "stm32f10x.h"

 
void InitAll(void)
{
   RCC->APB2ENR	|= (RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN); // Подали такты на порт А и альтернативные функции порта А. 
	                                                            //Без этого работать не будет.
				                                                      // Жоские вилы STM32 =))) А в симуляторе без этого работает на ура
   GPIOA->CRL	&= ~GPIO_CRL_CNF1;	   //Обнулили биты CNF1 т.к. после старта они настроены как (01)
	 GPIOA->CRL	&= ~GPIO_CRL_CNF1_1;	 // режим Open Drain. А нам надо альтернативный Push-Pull (10)

   GPIOA->CRL   |= GPIO_CRL_MODE1_1;	// Настроили порт на выход (для простоты возьмем тот факт, 
				                              // что GPIOA-CRL-MODE после сброса равны (00). Но в реальной программе 
				                              // эти моменты надо проверять и делать полную инициализацию порта. 
				                              // Согласно таблице 18 из R			
																			
	 RCC->APB1ENR |=	RCC_APB1ENR_TIM2EN ; //Разрешаем тактирование TIMER2
	 
	 TIM2->CCER |= TIM_CCER_CC2E ;         // Включаем на выход 2 канал таймера
	 TIM2->CCMR1 |=(TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2); // задаем прямой ШИМ.
	 TIM2->ARR = 499;
	 TIM2->PSC = 23;
	 TIM2->CNT = 249;
	 TIM2->CR1 |= TIM_CR1_ARPE ;
	 TIM2->CR1 |= TIM_CR1_CEN; //Запускаем таймер	

   return;
}

void delay(void) 
{
  volatile uint32_t i;
    for (i=1; i != 0xF000; i++);
  }



int main(void)
{
	
SystemInit ();	

InitAll();

TIM2->CCR2 = 249;
 
while(1)
	{
   
	 
	}
 
}
