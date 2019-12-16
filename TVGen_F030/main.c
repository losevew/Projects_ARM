/**
  ******************************************************************************
  *  This file configures the system clock as follows:
  *=============================================================================
  *=============================================================================
  *        System Clock source                    | PLL(HSE)
  *-----------------------------------------------------------------------------
  *        SYSCLK(Hz)                             | 48000000
  *-----------------------------------------------------------------------------
  *        HCLK(Hz)                               | 48000000
  *-----------------------------------------------------------------------------
  *        AHB Prescaler                          | 1
  *-----------------------------------------------------------------------------
  *        APB Prescaler                          | 1
  *-----------------------------------------------------------------------------
  *        HSE Frequency(Hz)                      | 8000000
  *----------------------------------------------------------------------------
  *        PLLMUL                                 | 6
  *-----------------------------------------------------------------------------
  *        PREDIV                                 | 1
  *-----------------------------------------------------------------------------
  *        Flash Latency(WS)                      | 1
  *-----------------------------------------------------------------------------
  *        Prefetch Buffer                        | ON
  *-----------------------------------------------------------------------------
  ******************************************************************************
  */
	
#include "stm32f0xx.h"


/** @addtogroup STM32F0xx_System_Private_Variables
  * @{
  */

volatile uint16_t synccount;	//  счетчик импульсов синхронизации
volatile int32_t delta, delta0;        // поправочные коэфициенты
volatile unsigned char phaselock; // флаг нахождения в фазе
volatile unsigned char phasecorrect; // флаг корректности фазы
volatile int16_t count; // счетчик проходов

static void SetSysClock(void);

#define HSE_STARTUP_TIMEOUT   ((uint16_t)0x5000) /*!< Time out for HSE start up */

//Тайминги
#define F_CPU (int32_t)48000000
#define Timer_WholeLine	(uint16_t)(F_CPU/15625) - 1		  			//One PAL line 64us
#define Timer_HalfLine	(uint16_t)((Timer_WholeLine + 1)/2) - 1		//Half PAL line 32us
#define Timer_ShortSync (uint16_t)((Timer_WholeLine + 1)/32) + 16  	//2,35us
#define Timer_NormalSync (uint16_t)((Timer_WholeLine + 12)/16)+ 32 	//4,7us
#define Timer_MidSync (uint16_t)((Timer_WholeLine + 1)*15/32) - 130 	//27.3us


#define Sync TIM16->CCR1

int main(void)
{ 
	synccount = 1;
	delta0 = 0;
	delta = 0; 
	phaselock = 0;
	phasecorrect = 0;
	count = 0;
	
	//инициализация системы тактирования
	SetSysClock();
	
	//включить тактирование портов и переферии  
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN ;
	RCC->APB2ENR |= RCC_APB2ENR_TIM16EN | RCC_APB2ENR_SYSCFGEN;
	
	/* Выбрать порт B для внешнего прерывания вывод 1, записав 0001 в EXTI0 (значение сброса) */
	SYSCFG->EXTICR[0] |= (uint16_t)SYSCFG_EXTICR1_EXTI1_PB ;
	
	/* Разрешить внешнее прерывание на линии 1 настроив соответствующий бит маски в регистре EXTI_IMR  */
	EXTI->IMR |= EXTI_IMR_MR1;
	/* Настроить биты выбора триггера линии прерывания на передний фронт*/
	EXTI->RTSR |= EXTI_RTSR_TR1;
	
	/* Конфигурирование NVIC таймера TIM16  и внешних прерываний */
	/* Разрешение прерываний от таймера TIM16 и  линий 0-1 внешних прерываний*/
	NVIC_EnableIRQ(TIM16_IRQn); 
	NVIC_EnableIRQ(EXTI0_1_IRQn);
	/* Установка преоритета  прерываний от таймера TIM16 и  линий 0-1 внешних прерываний*/
	NVIC_SetPriority(TIM16_IRQn,1); 
	NVIC_SetPriority(EXTI0_1_IRQn,0);
	
	
	/* подключите вывод к нужному AFx (AF5-0x05)в одном из GPIOx_AFRL(AFR[0]) или GPIOx_AFRH (AFR[1]) регистров
		настройки альтернативных функций для вывода PA6 распологаются начиная с 24 бита (6*4) регистра GPIOx_AFRL 
	включена функция AF5 (0x05) - прямой выход CH1 TIM16 */
	
	GPIOA->AFR[0] |= 0x05 << 24;
	
	/* выбрать тип, pull-up/pull-down  и скорость вывода через GPIOx_OTYPER, GPIOx_PUPDR и GPIOx_OSPEEDER, соответственно  
	для выводов PA4 и PA6 выбран режим выхода push-pull с высокой скоростью (50 Мгц)*/
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT_4 |GPIO_OTYPER_OT_6);
	GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR6 | GPIO_OSPEEDR_OSPEEDR4;
	
	/*настроить желаемый ввод-вывод в качестве альтернативной функции в регистре GPIOx_MODER для PA4 режим вывода, 
	для PA6 режим альтернативной функции*/
	GPIOA->MODER |= GPIO_MODER_MODER4_0; 
	GPIOA->MODER = (GPIOA->MODER & ~(GPIO_MODER_MODER6 )) | GPIO_MODER_MODER6_1; 
	
	//настройка таймера TIM16
	//Установитmь предварительный делитель на 1, при этом APBCLK / 1 т.e тактовая частота 48MHz  
	TIM16->PSC = 0;
	//Установитm ARR = Timer_HalfLine,  период составляет 32 us
	TIM16->ARR = Timer_HalfLine;
	//Установите CCR1 = Timer_MidSync, Начинаем с двух прерываний на строку.
	TIM16->CCR1 = Timer_MidSync;     
	//Выбрать режим PWM 2 на OC1 (OC1M = 111), включить регистр предварительной загрузки в OC1 (OC1PE = 1)
	TIM16->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_0 | TIM_CCMR1_OC1PE;
	//Выбрать активную высокую полярность в OC1 (CC1P = 0, значение сброса), включить выход на OC1 (CC1E = 1)
	TIM16->CCER |= TIM_CCER_CC1E;
	//Разрешить выход (MOE = 1)
	TIM16->BDTR |= TIM_BDTR_MOE;
	
	/* Включить счетчик (CEN = 1)
	Выбрать режим выравнивания по краям (CMS = 00, значение сброса)
	Выбрать направление как upcounter (DIR = 0, значение сброса) */
	TIM16->CR1 |= TIM_CR1_CEN;
	//Форсировать генерацию обновления (UG = 1)
	TIM16->EGR |= TIM_EGR_UG;
	
    //Включить прерывание по переполнению
	TIM16->DIER |= TIM_DIER_CC1IE ;

		
	while(1)
	{  
		if (phaselock) 
		{
            GPIOA->BSRR = GPIO_BSRR_BR_4; 
        }
        else  
		{ 
            GPIOA->BSRR = GPIO_BSRR_BS_4;
        }
		
	}


}

static void SetSysClock(void)
{
  __IO uint32_t StartUpCounter = 0, HSEStatus = 0;

/******************************************************************************/
/*            PLL (clocked by HSE) used as System clock source                */
/******************************************************************************/
  
  /* SYSCLK, HCLK, PCLK configuration ----------------------------------------*/
  /* Enable HSE */    
  RCC->CR |= ((uint32_t)RCC_CR_HSEON);
 
  /* Wait till HSE is ready and if Time out is reached exit */
  do
  {
    HSEStatus = RCC->CR & RCC_CR_HSERDY;
    StartUpCounter++;  
  } while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

  if ((RCC->CR & RCC_CR_HSERDY) != RESET)
  {
    HSEStatus = (uint32_t)0x01;
  }
  else
  {
    HSEStatus = (uint32_t)0x00;
  }  

  if (HSEStatus == (uint32_t)0x01)
  {
    /* Enable Prefetch Buffer and set Flash Latency */
    FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY;
 
    /* HCLK = SYSCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;
      
    /* PCLK = HCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE_DIV1;

    /* PLL configuration */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMUL));
    RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSE_PREDIV | RCC_CFGR_PLLXTPRE_HSE_PREDIV_DIV1 | RCC_CFGR_PLLMUL6);
            
    /* Enable PLL */
    RCC->CR |= RCC_CR_PLLON;	

    /* Wait till PLL is ready */
    while((RCC->CR & RCC_CR_PLLRDY) == 0)
    {
    }

    /* Select PLL as system clock source */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;    

    /* Wait till PLL is used as system clock source */
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)RCC_CFGR_SWS_PLL)
    {
    }
  }
  else
  { /* If HSE fails to start-up, the application will have wrong clock 
         configuration. User can add here some code to deal with this error */
  }  
}


//Обработка прерывания Hard Fault
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}


// Обработка прерывания таймера TIM16
void TIM16_IRQHandler(void)
{
	
	if ((TIM16->SR & TIM_SR_CC1IF) != 0)
	{
		switch (synccount) // формирование сигнала синхронизации по стандарту PAL
		{
			case 5://++++++++++++++++++++++++++++++++++++++++++++++++++++++++=
					TIM16->ARR = Timer_HalfLine + delta;
					Sync = Timer_ShortSync;
					synccount++;
			break;  
			case 11://++++++++++++++++++++++++++++++++++++++++++++++++++++++++
					TIM16->ARR = Timer_WholeLine ;
					Sync = Timer_NormalSync;
					synccount++;
			break;
			case 316://++++++++++++++++++++++++++++++++++++++++++++++++++++++++
					TIM16->ARR = Timer_HalfLine + delta;
					Sync = Timer_ShortSync;
					synccount++;	
			break;
			case 320://++++++++++++++++++++++++++++++++++++++++++++++++++++++++
					TIM16->ARR = Timer_HalfLine + delta;
					Sync = Timer_MidSync;
					synccount++;
			break;
			case 325://++++++++++++++++++++++++++++++++++++++++++++++++++++++++
					TIM16->ARR = Timer_HalfLine + delta;
					Sync = Timer_ShortSync;
					synccount++;
			break;
			case 330://++++++++++++++++++++++++++++++++++++++++++++++++++++++++
					TIM16->ARR = Timer_WholeLine ;
					Sync = Timer_NormalSync;
					synccount++;
			break;	
			case 635://++++++++++++++++++++++++++++++++++++++++++++++++++++++++
					TIM16->ARR = Timer_HalfLine + delta;
					Sync = Timer_ShortSync;
					synccount++;
			break;
			case 640://++++++++++++++++++++++++++++++++++++++++++++++++++++++++
					TIM16->ARR = Timer_HalfLine + delta;
					Sync = Timer_MidSync;
					synccount=1;
			break;

			default://++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			synccount++;
		}	
		
		TIM16->SR &= ~(TIM_SR_CC1IF ); /* Clear the flags */
										/* Очистить флаги */
		return;
		
	}
	else
	{
		/* Неожиданное прерывание */
		/* Управление ошибкой для надежного приложения */
		return;
	}
	
}

// Обработка внешнего прерывания на линиях 0-1
void EXTI0_1_IRQHandler(void)
{
	/*
	Обработа сигнала 1PPS
	при старте (сбросе) МК в течении первых 10 сек производится вычесление поправочного коэффициента delta0
	учитывающего нестабильность квацевого генератора (по сравнению с GPS)   и генерация синхросигнала начинается
	с 1 строки, учет поправочных коэффициентов производится путем изменения длительности выравнивающих импульсов
	*/
	if (phasecorrect == 0)
	{
		switch ( count)
		{
			case 0:
				
				TIM16->ARR = Timer_HalfLine;
				Sync = Timer_MidSync;
				synccount = 1;
				count++ ;
				TIM16->EGR |= TIM_EGR_UG;
				
			break;
			case 9:
				if (synccount>313)
				{
					delta0 = -(((640-(synccount-2))*Timer_WholeLine)/10)/750;
				}
				else
				{
					delta0 = (((synccount-5)*Timer_WholeLine)/10)/750;
				}
				
				phasecorrect = 1;
				count = 0;
				TIM16->ARR = Timer_HalfLine;
				Sync = Timer_MidSync;
				synccount = 1;
				delta = delta0;
				TIM16->EGR |= TIM_EGR_UG;
				
			break;
			
			default:
			count++ ;
		}
		
		EXTI->PR |= EXTI_PR_PR1;
		return;
	}
	
	
	if ((synccount < 10)||(synccount > 634)) 
	{ 
	/* если импульс 1 PPS пришел при генерации синхросигнала в диапазоне строк (импульсов) между 634 и 10,
		то выдается сигнал на светодиод о нахождении в фазе, если импульс пришел в диапазонах 10-25 и 615-634,
		то сигнал нахождения в фазе снимается и производится вычисление дополнительного поправочного 
		коэффициента delta который в дальнейшем учитывается при формировании синхросигнала
		при приходе импульса 1PPS вне указанных диапазонов снимается флаг phasecorrect и в течении 10сек
		производится начальная калибровка
	*/
      delta = delta0; 
      phaselock = 1;
	}
	else  
	{ 
		phaselock = 0;
		if (synccount <= 25) 
		{  
			delta = delta0 + ((synccount-5) * Timer_WholeLine)/750;
		}
		else  
		{
			if (synccount <= 615) 
			{    		
				synccount = 1;
				delta = 0; 
				phaselock = 0;
				phasecorrect = 0;			
			}
			else  
			{            
				delta = delta0 -((640-(synccount-2)) * Timer_WholeLine)/750 ;
			}
		}    
   }
	
	EXTI->PR |= EXTI_PR_PR1;
	return;
}


