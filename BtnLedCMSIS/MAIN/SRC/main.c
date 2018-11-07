#include "stm32f10x.h"

void Delay( uint32_t Val);
 
void InitAll(void)
{
   RCC->APB2ENR	|= (RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPAEN); // Подали такты на порт А и порт В. Без этого работать не будет.
				                                                      // Жоские вилы STM32 =))) А в симуляторе без этого работает на ура
   GPIOB->CRL	&= ~GPIO_CRL_CNF5;	//Обнулили биты CNF3 т.к. после старта они настроены как (01)
				                          // режим Open Drain. А нам надо Push-Pull (00)

   GPIOB->CRL   |= GPIO_CRL_MODE5_0;	// Настроили порт на выход (для простоты возьмем тот факт, 
				                              // что GPIOB-CRL-MODE после сброса равны (00). Но в реальной программе 
				                              // эти моменты надо проверять и делать полную инициализацию порта. 
				                              // Согласно таблице 18 из RM
																			
  	// Настройка входных портов. Те что на кнопки. 
		// Конфигурируем CRL регистры. 
		// Выставляем режим порта в CNF для битов 3 Режим 10 = PullUp(Down) Ставим первый бит CRL
		GPIOA->CRL	&= ~(GPIO_CRL_CNF3);	   				// Занулим заранее (режим входа)
		GPIOA->CRL 	|= GPIO_CRL_CNF3_1 ;            // включаем режим с подтяжкой
		GPIOA->BSRR = GPIO_BSRR_BS3;                // включаем подтяжку к питанию
																	

   return;
}



/* Тупая задержка */
void Delay( uint32_t Val) 
{
for( ; Val != 0; Val--) 
  		{
		__nop();
  		}
}



int main(void)
{
InitAll();
 
while(1)
	{
   	if ((GPIOA->IDR & GPIO_IDR_IDR3)==0x00) // проверяем нажатие кнопки
		{
			GPIOB->BSRR = GPIO_BSRR_BS5;		// Выставили бит 5.  GPIO_BSRR_BS5 это битмаска
			Delay(200000);
			GPIOB->BSRR = GPIO_BSRR_BR5;		// Сбросили бит 5
			Delay(200000);
		}	
	}
 
}
