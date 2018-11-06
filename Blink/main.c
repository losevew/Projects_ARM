#include "stm32f10x.h"

void InitAll(void)
{
//		RCC->APB2ENR |= RCC_APB2ENR_IOPCEN; // Подали такты на порт. Без этого работать не будет.
//																			  // Жоские вилы STM32 =))) А в симуляторе без этого работает на ура
//		GPIOC->CRL &= ~GPIO_CRL_CNF3;       // Обнулили биты CNF3 т.к. после старта они настроены как (01)
//																	      // режим Open Drain. А нам надо Push-Pull (00)
//		GPIOC->CRL |= GPIO_CRL_MODE3_0;     // Настроили порт на выход (для простоты возьмем тот факт,
//																		    // что GPIOA-CRL-MODE после сброса равны (00). Но в реальной программе
//																		    // эти моменты надо проверять и делать полную инициализацию порта.
//																	      // Согласно таблице 18 из RM
//	
	
		GPIO_InitTypeDef  GPIO_InitStructure;
	 
		/* Initialize LED which connected to PC13 */
		// Enable PORTC Clock
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
		/* Configure the GPIO_LED pin */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
		GPIO_ResetBits(GPIOC, GPIO_Pin_13); // Set C13 to Low level ("0")
		//GPIO_SetBits(GPIOC, GPIO_Pin_13); // Set C13 to High level ("1")
		return;
}

int main(void)
	
{ 
	int i;	
	InitAll();
		while(1)
		{
				GPIOC->BSRR = GPIO_BSRR_BS13; // Выставили бит 13. GPIO_BSRR_BS3 это битмаска
			
				/* delay */
        for(i=0;i<0x150000;i++);
				GPIOC->BSRR = GPIO_BSRR_BR13; // Сбросили бит 13
			  /* delay */
        for(i=0;i<0x150000;i++);
		}
	}

