/**
  ******************************************************************************
  * @file    system_stm32f0xx.c
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    24-July-2014
  * @brief   CMSIS Cortex-M0 Device Peripheral Access Layer System Source File.
  *          This file contains the system clock configuration for STM32F0xx devices,
  *          and is generated by the clock configuration tool  
  *          STM32F0xx_Clock_Configuration_V1.0.1.xls
  *
  * 1.  This file provides two functions and one global variable to be called from 
  *     user application:
  *      - SystemInit(): Setups the system clock (System clock source, PLL Multiplier
  *                      and Divider factors, AHB/APBx prescalers and Flash settings),
  *                      depending on the configuration made in the clock xls tool.
  *                      This function is called at startup just after reset and 
  *                      before branch to main program. This call is made inside
  *                      the "startup_stm32f0xx.s" file.
  *
  *      - SystemCoreClock variable: Contains the core clock (HCLK), it can be used
  *                                  by the user application to setup the SysTick 
  *                                  timer or configure other parameters.
  *
  *      - SystemCoreClockUpdate(): Updates the variable SystemCoreClock and must
  *                                 be called whenever the core clock is changed
  *                                 during program execution.
  *
  * 2. After each device reset the HSI (8 MHz Range) is used as system clock source.
  *    Then SystemInit() function is called, in "startup_stm32f0xx.s" file, to
  *    configure the system clock before to branch to main program.
  *
  * 3. If the system clock source selected by user fails to startup, the SystemInit()
  *    function will do nothing and HSI still used as system clock source. User can 
  *    add some code to deal with this issue inside the SetSysClock() function.
  *
  * 4. The default value of HSE crystal is set to 8MHz, refer to "HSE_VALUE" define
  *    in "stm32f0xx.h" file. When HSE is used as system clock source, directly or
  *    through PLL, and you are using different crystal you have to adapt the HSE
  *    value to your own configuration.
  *
  * 5. This file configures the system clock as follows:
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
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */
	
#include "stm32f0xx.h"


/** @addtogroup STM32F0xx_System_Private_Variables
  * @{
  */
uint16_t Counter = 0;

static void SetSysClock(void);

#define HSE_STARTUP_TIMEOUT   ((uint16_t)0x5000) /*!< Time out for HSE start up */


int main(void)
{
	int i;  
	//������������� ������� ������������
	SetSysClock();
	
	/* ���������������� NVIC ������� TIM16 */
	/* ���������� ���������� �� ������� TIM16 */
	NVIC_EnableIRQ(TIM16_IRQn); 
	
	 /* ��������� ����������  ���������� �� ������� TIM16*/
	NVIC_SetPriority(TIM16_IRQn,0); /* (2) */
	
	//�������� ������������ ������ � ���������  
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	RCC->APB2ENR |= RCC_APB2ENR_TIM16EN;
	
	/* ���������� ���� / ����� � ������� AFx (AF5-0x05)� ����� �� GPIOx_AFRL(AFR[0]) ��� GPIOx_AFRH (AFR[1]) ���������
		��������� �������������� ������� ��� ������ PA6 ������������� ������� � 24 ���� (6*4) �������� GPIOx_AFRL 
	�������� ������� AF5 (0x05) - ������ ����� CH1 TIM16 */
	
	GPIOA->AFR[0] |= 0x05 << 24;
	
	/* �������� ���, pull-up/pull-down  � �������� ������ ����� GPIOx_OTYPER, GPIOx_PUPDR � GPIOx_OSPEEDER, ��������������  
	��� ������� PA4 � PA6 ������ ����� ������ push-pull � ������� ��������� (50 ���)*/
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT_4 |GPIO_OTYPER_OT_6);
	GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR6 | GPIO_OSPEEDR_OSPEEDR4;
	
	/*��������� �������� ����-����� � �������� �������������� ������� � �������� GPIOx_MODER ��� PA4 ����� ������, 
	��� PA6 ����� �������������� �������*/
	GPIOA->MODER |= GPIO_MODER_MODER4_0; 
	GPIOA->MODER = (GPIOA->MODER & ~(GPIO_MODER_MODER6 )) | GPIO_MODER_MODER6_1; 
	
	
	//���������m� ��������������� �������� �� 47, ������� APBCLK / 48 i.e 1MHz  
	TIM16->PSC = 47;
	//���������m ARR = 99, ��� ��� ������ ���������� 1 ���, ������ ���������� 100 us
	TIM16->ARR = 99;
	//���������� CCRx = 40, ������ ����� ������� � ������� 40 us
	TIM16->CCR1 = 40;
	//������� ����� PWM 1 �� OC1 (OC1M = 110), �������� ������� ��������������� �������� � OC1 (OC1PE = 1)
	TIM16->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1PE;
	//������� �������� ������� ���������� � OC1 (CC1P = 0, �������� ������), �������� ����� �� OC1 (CC1E = 1)
	TIM16->CCER |= TIM_CCER_CC1E;
	//��������� ����� (MOE = 1)
	TIM16->BDTR |= TIM_BDTR_MOE;
	//�������� ���������� �� ������������
	TIM16->DIER |= TIM_DIER_CC1IE ;
	/* �������� ������� (CEN = 1)
	������� ����� ������������ �� ����� (CMS = 00, �������� ������)
	������� ����������� ��� upcounter (DIR = 0, �������� ������) */
	TIM16->CR1 |= TIM_CR1_CEN;
	//����������� ��������� ���������� (UG = 1)
	TIM16->EGR |= TIM_EGR_UG;
	
    

		
	while(1)
	{  
		
		GPIOA->BSRR = GPIO_BSRR_BS_4;
		/* delay */
    for(i=0;i<0x150000;i++);

		GPIOA->BSRR = GPIO_BSRR_BR_4;
		/* delay */
    for(i=0;i<0x80000;i++);
		
		
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


//��������� ���������� Hard Fault
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}


// ��������� ���������� ������� TIM16
void TIM16_IRQHandler(void)
{
	
	if ((TIM16->SR & TIM_SR_CC1IF) != 0)
	{
		if ( Counter < 2)
		{
			TIM16->CCR1 = 20;
			Counter++ ;
		}
		else
		{
			TIM16->CCR1 = 60;
			Counter++ ;
		}
		if (Counter == 4 )
		{
			Counter = 0;
		}			
		
		TIM16->SR &= ~(TIM_SR_CC1OF | TIM_SR_CC1IF); /* Clear the flags */
														/* �������� ����� */
		return;
		
	}
	else
	{
		/* ����������� ���������� */
		/* ���������� ������� ��� ��������� ���������� */
		
		return;
	}
	
}
