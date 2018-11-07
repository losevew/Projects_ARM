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

#include "5510.h"

static __IO uint32_t TimingDelay = 0;
void Configure_Systick(void);
void SysTick_Handler(void);
void Delay_US(__IO uint32_t nTime);
static void SetSysClock(void);

#define HSE_STARTUP_TIMEOUT   ((uint16_t)0x5000) /*!< Time out for HSE start up */

N5110_Bitmap_TypeDef buff;

N5110_TypeDef LCD = {
		Delay_US,					// Delay Function Name
		&buff
};


int main(void)
{
	uint8_t i;
	uint8_t str[10] = "how.net.ua";

	//инициализация системы тактирования
	SetSysClock();	

	Configure_Systick();

	N5110_Init(&LCD);

	N5110_WriteChar(&LCD, 5, 5, 'A');
	N5110_WriteCharInv(&LCD, 10, 15, 'B');
	N5110_WriteString(&LCD, 10, 40, str);

	//Delay_US(2000);
	N5110_Clear(&LCD);


    while(1)
    {
		N5110_DrawCircle(&LCD, 20, 23, 15);
		N5110_DrawCircle(&LCD, 65, 23, 15);
		N5110_DrawRect(&LCD, 0, 0, 83, 47);
		N5110_DrawRect(&LCD, 9, 0, 71, 5);

		N5110_GotoXY(&LCD, 10, 40);
		N5110_WriteString(&LCD, 10, 40, str);

    	for (i=0;i<60;i++){
    		N5110_DrawClock(&LCD, 20, 23, 13, i);
    		N5110_DrawClock(&LCD, 65, 23, 13, 59-i);
    		N5110_DrawPixel(&LCD, 11+i, 2);
    		N5110_DrawPixel(&LCD, 11+i, 3);
    		Delay_US(50);
    	}
    	N5110_Clear(&LCD);
    }
}



__INLINE void Configure_Systick(void)
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
