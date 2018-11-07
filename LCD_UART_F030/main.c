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
void Configure_Systick(void);
static void SetSysClock(void);
void Delay_US(__IO uint32_t nTime);
static void SetSysClock(void);

#define HSE_STARTUP_TIMEOUT   ((uint16_t)0x5000) /*!< Time out for HSE start up */


int main(void)
{ 
	
	
	//инициализация системы тактирования
	SetSysClock();
	Configure_Systick();
	
	lcd8544_init(); // инициализация дисплея

  lcd8544_rect(1,1,82,46,1); // прямоугольник

  unsigned char str1[]="PCD8544";
  unsigned char str2[]="НА STM32F0XX";
  lcd8544_putstr(22, 10, str1, 1); // вывод первой строки
  lcd8544_putstr( 6, 18, str2, 1); // вывод второй строки

    lcd8544_refresh(); // вывод буфера на экран ! без этого ничего видно не будет !
	
	Delay_US(1500);
	lcd8544_clear();
	
	//str1[] = "МОЙ";
    //str2[] = "ВАРИАНТ";
	  lcd8544_rect(1,1,82,46,1); // прямоугольник
    lcd8544_putstr(32, 10, "МОЙ", 1); // вывод первой строки
    lcd8544_putstr( 24, 18, "ВАРИАНТ", 1); // вывод второй строки

    lcd8544_refresh();
	
	Delay_US(1500);
	lcd8544_clear();
	

	while(1)
	{  
		lcd8544_rect(1,1,82,46,1); // прямоугольник
		lcd8544_putstr(22, 10, str1, 1); // вывод первой строки
		lcd8544_putstr( 6, 18, str2, 1); // вывод второй строки

		lcd8544_refresh(); // вывод буфера на экран ! без этого ничего видно не будет !
		
		Delay_US(1500);
		lcd8544_clear();
		
		lcd8544_rect(1,1,82,46,1); // прямоугольник
		lcd8544_putstr(32, 10, "МОЙ", 1); // вывод первой строки
		lcd8544_putstr( 24, 18, "ВАРИАНТ", 1); // вывод второй строки
		lcd8544_dec(123, 3, 22, 26, 1);
		lcd8544_putchar(46,26,'2',1);
		lcd8544_line(10,32,64,38,1);
		lcd8544_fillrect(8,38,72,44,1);

		lcd8544_refresh();
		Delay_US(1500);
		lcd8544_clear();
		
	}

}

static void Configure_Systick(void)
{
  SysTick_Config(48000); /* 1ms config */
}

void Delay_US(__IO uint32_t nTime)
{
  TimingDelay = nTime;
  while(TimingDelay != 0);
}

void SysTick_Handler()
{
	if (TimingDelay != 0) TimingDelay--;
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


