#include "stm32f10x.h"


void InitAll(void)
{
		RCC->APB2ENR |= RCC_APB2ENR_IOPCEN; // Подали такты на порт. Без этого работать не будет.
  																		  // Жоские вилы STM32 =))) А в симуляторе без этого работает на ура

		GPIOC->CRH &= ~GPIO_CRH_CNF13; 			// Обнулили биты CNF13 т.к. после старта они настроены как (01)		
																				// режим Open Drain. А нам надо Push-Pull (00)
		GPIOC->CRH |= GPIO_CRH_MODE13_0; 		// Настроили порт на выход (для простоты возьмем тот факт,
																		    // что GPIOC-CRH-MODE после сброса равны (00). Но в реальной программе
																				// эти моменты надо проверять и делать полную инициализацию порта.
																				// Согласно таблице 18 из RM
		return;
}
int main(void)
{ 
			InitAll();
			int i;
			while(1)
			{
				GPIOC->BSRR = GPIO_BSRR_BS13; // Выставили бит 13. GPIO_BSRR_BS13 это битмаска		
					/* delay */
        for(i=0;i<0x120000;i++);
				
				GPIOC->BSRR = GPIO_BSRR_BR13; // Сбросили бит 13
				/* delay */
        for(i=0;i<0x120000;i++);
			}
}
