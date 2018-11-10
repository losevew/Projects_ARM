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
#include "f10x-pcd8544.h" // смотрите этот файл для настройки дисплея !

static __IO uint32_t TimingDelay = 0;
uint8_t send = 0;
const uint8_t stringtosend[] = "Hello, World!\n";


void Configure_Systick(void);
static void SetSysClock(void);
void Configure_GPIO_USART1(void);
void Configure_USART1(void);
void Configure_GPIO_Button(void);
void Configure_EXTI(void);
void Delay_US(__IO uint32_t nTime);
static void SetSysClock(void);

#define HSE_STARTUP_TIMEOUT   ((uint16_t)0x5000) /*!< Time out for HSE start up */


int main(void)
{ 
	
	
	//инициализация системы тактирования
	SetSysClock();
	Configure_Systick();
	
	//инициализация USART
	Configure_GPIO_USART1();
  Configure_USART1();
	
	Configure_GPIO_Button();
  Configure_EXTI();
	
	lcd8544_init(); // инициализация дисплея

  lcd8544_rect(1,1,82,46,1); // прямоугольник

  unsigned char str1[]="PCD8544";
  unsigned char str2[]="НА STM32F0XX";
  lcd8544_putstr(22, 10, str1, 1); // вывод первой строки
  lcd8544_putstr( 6, 18, str2, 1); // вывод второй строки

  lcd8544_refresh(); // вывод буфера на экран ! без этого ничего видно не будет !
	
	Delay_US(1500);
	lcd8544_clear();
	
	lcd8544_rect(1,1,82,46,1); // прямоугольник
  lcd8544_putstr(32, 10, "Проверка Usart", 1); // вывод первой строки
  lcd8544_refresh();
	
//	Delay_US(1500);
//	lcd8544_clear();
	

	while(1)
	{  
		
		
	}

}

//-----------------------------------------------------------
// Кофигурация системного таймера
//-----------------------------------------------------------

static void Configure_Systick(void)
{
  SysTick_Config(48000); /* 1ms config */
}

//-----------------------------------------------------------
// Функция формирования задержки
//-----------------------------------------------------------

void Delay_US(__IO uint32_t nTime)
{
  TimingDelay = nTime;
  while(TimingDelay != 0);
}

//------------------------------------------------------------
// Функция обработки прерывания от системного таймера
//------------------------------------------------------------

void SysTick_Handler()
{
	if (TimingDelay != 0) TimingDelay--;
}

//------------------------------------------------------------
// Функция обработки прерывания от USART1
//------------------------------------------------------------

void USART1_IRQHandler(void)
{
  uint8_t chartoreceive = 0;
  
  if((USART1->ISR & USART_ISR_RXNE) == USART_ISR_RXNE)
  {
    chartoreceive = (uint8_t)(USART1->RDR); /* Прием данных, очистка флага */
          
  }
  else
  {
     NVIC_DisableIRQ(USART1_IRQn); /* Отключение USART1_IRQn */
  }
	
	
	if((USART1->ISR & USART_ISR_TC) == USART_ISR_TC)
  {
    if(send == sizeof(stringtosend))
    {
      send=0;
      USART1->ICR |= USART_ICR_TCCF; /* Очистка флага завершения передачи */
    }
    else
    {
      /* Очистка флага завершения передачи и загрузка в TDR нового символа */
      USART1->TDR = stringtosend[send++];
    }
  }
  else
  {
      NVIC_DisableIRQ(USART1_IRQn); /* отключение USART1_IRQn */
  }
	
}

//-----------------------------------------------------------------------
// Функция обработки внешнего прерывания от вывода А0
//-----------------------------------------------------------------------
void EXTI0_1_IRQHandler(void)
{
  EXTI->PR |= 1;	
	
  /* Начало передачи USART */
  USART1->TDR = stringtosend[send++]; /* Will inititiate TC if TXE */
}

//-----------------------------------------------------------------------
// Функция конфигурирования портов USART1
//-----------------------------------------------------------------------

__INLINE void Configure_GPIO_USART1(void)
{
  /* разрешить тактирование GPIOA */
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	
  /* Конфигурирование GPIO для сигналов  USART1 */
  /* (1) Выбрать AF mode (10) для PA9 и PA10 */
  /* (2) Выбрать AF1 для сигналов USART1 */
  GPIOA->MODER = (GPIOA->MODER & ~(GPIO_MODER_MODER9|GPIO_MODER_MODER10))\
                 | (GPIO_MODER_MODER9_1 | GPIO_MODER_MODER10_1); /* (1) */
  GPIOA->AFR[1] = (GPIOA->AFR[1] &~ (GPIO_AFRH_AFRH1 | GPIO_AFRH_AFRH2))\
                  | (1 << (1 * 4)) | (1 << (2 * 4)); /* (2) */
}

//------------------------------------------------------------------------
// Функция конфигурирования USART1
//------------------------------------------------------------------------

__INLINE void Configure_USART1(void)
{
  /* разрешить тактирование USART1 */
  RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

  /* Конфигурирование USART1 */
  /* (1) oversampling by 16, 9600 бод */
  /* (2) слово 8 бит, 1 стартовый бит, 1 стоп бит, no parity */
  USART1->BRR = 480000 / 96; /* (1) */
  USART1->CR1 = USART_CR1_RXNEIE | USART_CR1_RE | USART_CR1_TE | USART_CR1_UE; /* (2) */
  
  /* опрос передатчика*/
  while((USART1->ISR & USART_ISR_TC) != USART_ISR_TC)
  { 
    /* add time out here for a robust application */
  }
  USART1->ICR |= USART_ICR_TCCF;/* очистка флага  TC */
  USART1->CR1 |= USART_CR1_TCIE;/* разрешение прерывания TC */
  
  /* Конфигурирование прерываний */
  /* (3) Установить приоритет для USART1_IRQn */
  /* (4) Разрешить прерывание USART1_IRQn */
  NVIC_SetPriority(USART1_IRQn, 0); /* (3) */
  NVIC_EnableIRQ(USART1_IRQn); /* (4) */
}

//---------------------------------------------------------------------------
// Функция конфигурирования порта кнопки
//---------------------------------------------------------------------------

__INLINE void Configure_GPIO_Button(void)
{
  /* разрешить тактирование GPIOA */
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	
  /* Выбор режима */
  /* Выбрать режим входа(00) для PA0 */
  GPIOA->MODER = (GPIOA->MODER & ~(GPIO_MODER_MODER0));
}

//-----------------------------------------------------------------------------
// Функция конфигурирования внешнего прерывания
//------------------------------------------------------------------------

__INLINE void Configure_EXTI(void)
{
  /* конфигурирования Syscfg, exti и nvic для  PA0 */
  /* (1) PA0 как вход прерывания */
  /* (2) unmask port 0 */
  /* (3) передний фронт */
  /* (4) установить приоритет */
  /* (5) разрешить прерывание EXTI0_1_IRQn */
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
  SYSCFG->EXTICR[0] = (SYSCFG->EXTICR[0] & ~SYSCFG_EXTICR1_EXTI0) | SYSCFG_EXTICR1_EXTI0_PA; /* (1) */ 
  EXTI->IMR |= EXTI_IMR_MR0; /* (2) */ 
  EXTI->RTSR |= EXTI_RTSR_TR0; /* (3) */ 
  NVIC_SetPriority(EXTI0_1_IRQn, 0); /* (4) */ 
  NVIC_EnableIRQ(EXTI0_1_IRQn); /* (5) */ 
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


