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

volatile uint16_t synccount;	//  ������� ��������� �������������
volatile int32_t delta, delta0;        // ����������� �����������
volatile unsigned char phaselock; // ���� ���������� � ����
volatile unsigned char phasecorrect; // ���� ������������ ����
volatile int16_t count; // ������� ��������

static void SetSysClock(void);

#define HSE_STARTUP_TIMEOUT   ((uint16_t)0x5000) /*!< Time out for HSE start up */

//��������
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
	
	//������������� ������� ������������
	SetSysClock();
	
	//�������� ������������ ������ � ���������  
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN ;
	RCC->APB2ENR |= RCC_APB2ENR_TIM16EN | RCC_APB2ENR_SYSCFGEN;
	
	/* ������� ���� B ��� �������� ���������� ����� 1, ������� 0001 � EXTI0 (�������� ������) */
	SYSCFG->EXTICR[0] |= (uint16_t)SYSCFG_EXTICR1_EXTI1_PB ;
	
	/* ��������� ������� ���������� �� ����� 1 �������� ��������������� ��� ����� � �������� EXTI_IMR  */
	EXTI->IMR |= EXTI_IMR_MR1;
	/* ��������� ���� ������ �������� ����� ���������� �� �������� �����*/
	EXTI->RTSR |= EXTI_RTSR_TR1;
	
	/* ���������������� NVIC ������� TIM16  � ������� ���������� */
	/* ���������� ���������� �� ������� TIM16 �  ����� 0-1 ������� ����������*/
	NVIC_EnableIRQ(TIM16_IRQn); 
	NVIC_EnableIRQ(EXTI0_1_IRQn);
	/* ��������� ����������  ���������� �� ������� TIM16 �  ����� 0-1 ������� ����������*/
	NVIC_SetPriority(TIM16_IRQn,1); 
	NVIC_SetPriority(EXTI0_1_IRQn,0);
	
	
	/* ���������� ����� � ������� AFx (AF5-0x05)� ����� �� GPIOx_AFRL(AFR[0]) ��� GPIOx_AFRH (AFR[1]) ���������
		��������� �������������� ������� ��� ������ PA6 ������������� ������� � 24 ���� (6*4) �������� GPIOx_AFRL 
	�������� ������� AF5 (0x05) - ������ ����� CH1 TIM16 */
	
	GPIOA->AFR[0] |= 0x05 << 24;
	
	/* ������� ���, pull-up/pull-down  � �������� ������ ����� GPIOx_OTYPER, GPIOx_PUPDR � GPIOx_OSPEEDER, ��������������  
	��� ������� PA4 � PA6 ������ ����� ������ push-pull � ������� ��������� (50 ���)*/
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT_4 |GPIO_OTYPER_OT_6);
	GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR6 | GPIO_OSPEEDR_OSPEEDR4;
	
	/*��������� �������� ����-����� � �������� �������������� ������� � �������� GPIOx_MODER ��� PA4 ����� ������, 
	��� PA6 ����� �������������� �������*/
	GPIOA->MODER |= GPIO_MODER_MODER4_0; 
	GPIOA->MODER = (GPIOA->MODER & ~(GPIO_MODER_MODER6 )) | GPIO_MODER_MODER6_1; 
	
	//��������� ������� TIM16
	//���������m� ��������������� �������� �� 1, ��� ���� APBCLK / 1 �.e �������� ������� 48MHz  
	TIM16->PSC = 0;
	//���������m ARR = Timer_HalfLine,  ������ ���������� 32 us
	TIM16->ARR = Timer_HalfLine;
	//���������� CCR1 = Timer_MidSync, �������� � ���� ���������� �� ������.
	TIM16->CCR1 = Timer_MidSync;     
	//������� ����� PWM 2 �� OC1 (OC1M = 111), �������� ������� ��������������� �������� � OC1 (OC1PE = 1)
	TIM16->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_0 | TIM_CCMR1_OC1PE;
	//������� �������� ������� ���������� � OC1 (CC1P = 0, �������� ������), �������� ����� �� OC1 (CC1E = 1)
	TIM16->CCER |= TIM_CCER_CC1E;
	//��������� ����� (MOE = 1)
	TIM16->BDTR |= TIM_BDTR_MOE;
	
	/* �������� ������� (CEN = 1)
	������� ����� ������������ �� ����� (CMS = 00, �������� ������)
	������� ����������� ��� upcounter (DIR = 0, �������� ������) */
	TIM16->CR1 |= TIM_CR1_CEN;
	//����������� ��������� ���������� (UG = 1)
	TIM16->EGR |= TIM_EGR_UG;
	
    //�������� ���������� �� ������������
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
		switch (synccount) // ������������ ������� ������������� �� ��������� PAL
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

// ��������� �������� ���������� �� ������ 0-1
void EXTI0_1_IRQHandler(void)
{
	/*
	�������� ������� 1PPS
	��� ������ (������) �� � ������� ������ 10 ��� ������������ ���������� ������������ ������������ delta0
	������������ �������������� ��������� ���������� (�� ��������� � GPS)   � ��������� ������������� ����������
	� 1 ������, ���� ����������� ������������� ������������ ����� ��������� ������������ ������������� ���������
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
	/* ���� ������� 1 PPS ������ ��� ��������� ������������� � ��������� ����� (���������) ����� 634 � 10,
		�� �������� ������ �� ��������� � ���������� � ����, ���� ������� ������ � ���������� 10-25 � 615-634,
		�� ������ ���������� � ���� ��������� � ������������ ���������� ��������������� ������������ 
		������������ delta ������� � ���������� ����������� ��� ������������ �������������
		��� ������� �������� 1PPS ��� ��������� ���������� ��������� ���� phasecorrect � � ������� 10���
		������������ ��������� ����������
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


