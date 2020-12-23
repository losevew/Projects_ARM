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
#include "f10x-pcd8544.h" // �������� ���� ���� ��� ��������� ������� !

#define HSE_STARTUP_TIMEOUT   ((uint16_t)0x5000) /*!< Time out for HSE start up */


// USART Receiver buffer
#define RX_BUFFER_SIZE 8

uint8_t rx_data[RX_BUFFER_SIZE];
unsigned char rx_count=0;

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
//void Usart_Transmit(uint8_t Data);
//void USART1_str (const char * data);
void LCD_Out_RX (void);
void LCD_Out_TX_completed (void);

unsigned char FRT = 0;                //���� ������� ��������
unsigned char FTC = 0;                //���� ��������� ��������
unsigned char FOD = 0;                //���� ����������� ����������



int main(void)
{ 
	
	
	//������������� ������� ������������
	SetSysClock();
	Configure_Systick();
	
	//������������� USART
	Configure_GPIO_USART1();
	Configure_USART1();
	
	Configure_GPIO_Button();
	Configure_EXTI();
	
	lcd8544_init(); // ������������� �������

	lcd8544_rect(1,1,82,47,1); // �������������

	lcd8544_putstr(24, 10, "PCD8544", 1); // ����� ������ ������
	lcd8544_putstr(12, 20, "�� STM32F0", 1); // ����� ������ ������

	lcd8544_refresh(); // ����� ������ �� ����� ! ��� ����� ������ ����� �� ����� !
		
	Delay_US(1500);
	lcd8544_clear();
	
	lcd8544_rect(0,0,82,47,1); // �������������
	lcd8544_putstr(12, 10, "���� USART", 1); // ����� ������ ������
	lcd8544_refresh();
		
//	Delay_US(1500);
//	lcd8544_clear();
//	

	while(1)
	{  
		if (FOD==1)
        {
            LCD_Out_RX();
        }
		if (FTC==1)
        {
            LCD_Out_TX_completed();
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

//-----------------------------------------------------------
// ����������� ���������� �������
//-----------------------------------------------------------

static void Configure_Systick(void)
{
  SysTick_Config(48000); /* 1ms config */
}


//-----------------------------------------------------------------------
// ������� ���������������� ������ USART1
//-----------------------------------------------------------------------

__INLINE void Configure_GPIO_USART1(void)
{
  /* ��������� ������������ GPIOA */
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	
  /* ���������������� GPIO ��� ��������  USART1 */
  /* (1) ������� AF mode (10) ��� PA9 � PA10 */
  /* (2) ������� AF1 ��� �������� USART1 */
  GPIOA->MODER = (GPIOA->MODER & ~(GPIO_MODER_MODER9|GPIO_MODER_MODER10))\
                 | (GPIO_MODER_MODER9_1 | GPIO_MODER_MODER10_1); /* (1) */
  GPIOA->AFR[1] = (GPIOA->AFR[1] &~ (GPIO_AFRH_AFRH1 | GPIO_AFRH_AFRH2))\
                  | (1 << (1 * 4)) | (1 << (2 * 4)); /* (2) */
}

//------------------------------------------------------------------------
// ������� ���������������� USART1
//------------------------------------------------------------------------

__INLINE void Configure_USART1(void)
{
  /* ��������� ������������ USART1 */
  RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

  /* ���������������� USART1 */
  /* (1) oversampling by 16, 9600 ��� */
  /* (2) ����� 8 ���, 1 ��������� ���, 1 ���� ���, no parity */
  USART1->BRR = 480000 / 96; /* (1) */
  USART1->CR1 = USART_CR1_RXNEIE | USART_CR1_RE | USART_CR1_TE | USART_CR1_UE; /* (2) */
  
  /* ����� �����������*/
  while((USART1->ISR & USART_ISR_TC) != USART_ISR_TC)
  { 
    /* add time out here for a robust application */
  }
  USART1->ICR |= USART_ICR_TCCF;/* ������� �����  TC */
  USART1->CR1 |= USART_CR1_TCIE;/* ���������� ���������� TC */
  
  /* ���������������� ���������� */
  /* (3) ���������� ��������� ��� USART1_IRQn */
  /* (4) ��������� ���������� USART1_IRQn */
  NVIC_SetPriority(USART1_IRQn, 0); /* (3) */
  NVIC_EnableIRQ(USART1_IRQn); /* (4) */
}

//---------------------------------------------------------------------------
// ������� ���������������� ����� ������
//---------------------------------------------------------------------------

__INLINE void Configure_GPIO_Button(void)
{
  /* ��������� ������������ GPIOA */
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	
  /* ����� ������ */
  /* ������� ����� �����(00) ��� PA0 */
  GPIOA->MODER = (GPIOA->MODER & ~(GPIO_MODER_MODER0));
}

//-----------------------------------------------------------------------------
// ������� ���������������� �������� ����������
//------------------------------------------------------------------------

__INLINE void Configure_EXTI(void)
{
	/* ���������������� Syscfg, exti � nvic ���  PA0 */
	/* (1) PA0 ��� ���� ���������� */
	/* (2) unmask port 0 */
	/* (3) �������� ����� */
	/* (4) ���������� ��������� */
	/* (5) ��������� ���������� EXTI0_1_IRQn */
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	SYSCFG->EXTICR[0] = (SYSCFG->EXTICR[0] & ~SYSCFG_EXTICR1_EXTI0) | SYSCFG_EXTICR1_EXTI0_PA; /* (1) */ 
	EXTI->IMR |= EXTI_IMR_MR0; /* (2) */ 
	EXTI->RTSR |= EXTI_RTSR_TR0; /* (3) */ 
	NVIC_SetPriority(EXTI0_1_IRQn, 0); /* (4) */ 
	NVIC_EnableIRQ(EXTI0_1_IRQn); /* (5) */ 
}





/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

//------------------------------------------------------------
// ������� ��������� ���������� �� ���������� �������
//------------------------------------------------------------

void SysTick_Handler()
{
	if (TimingDelay != 0) TimingDelay--;
}

//------------------------------------------------------------
// ������� ��������� ���������� �� USART1
//------------------------------------------------------------

void USART1_IRQHandler(void)
{
  //uint8_t chartoreceive = 0;
  
  if((USART1->ISR & USART_ISR_RXNE) == USART_ISR_RXNE)
  {
    rx_data[rx_count] = (uint8_t)(USART1->RDR); /* ����� ������, ������� ����� */
	rx_count++;
	if(rx_count==RX_BUFFER_SIZE)
	{
		rx_count=0;
		FOD = 1;  	
	}
    return;  
  }
	
 if(USART1->ISR & USART_ISR_TC){ // �������� ���������
		USART1->ICR |= USART_ICR_TCCF; /* ������� ����� ���������� �������� */
		FTC = 1;
    }
    if(USART1->ISR & USART_ISR_TXE){ // TX ������ - ��������� ��������� ������
        // ��������, ������� � TDR
        if(send < sizeof(stringtosend)-1){
            USART1->TDR = stringtosend[send++];
        }else{
			send=0;	
			USART1->CR1 &= ~USART_CR1_TXEIE; // ��������� ����������
		}
    } 		
	
}

//-----------------------------------------------------------------------
// ������� ��������� �������� ���������� �� ������ �0
//-----------------------------------------------------------------------
void EXTI0_1_IRQHandler(void)
{
  EXTI->PR |= 1;	
	
  /* ������ �������� USART */
  USART1->TDR = stringtosend[send++]; /* Will inititiate TC if TXE */
}


//-----------------------------------------------------------
// ������� ������������ ��������
//-----------------------------------------------------------

void Delay_US(__IO uint32_t nTime)
{
  TimingDelay = nTime;
  while(TimingDelay != 0);
}

//-----------------------------------------------------------------------
// ������� �������� �����
//-----------------------------------------------------------------------

//void Usart_Transmit(uint8_t Data)
//{
//  while(!(USART1->ISR & USART_ISR_TXE));
//  USART1->TDR = Data;
//}


//-----------------------------------------------------------------------
// ������� �������� ������
//-----------------------------------------------------------------------

//void USART1_str (const char * data)
//{

//	while((*data )){ 
//	  Usart_Transmit (*data);
//    data++;
//	  }
//}

void LCD_Out_RX (void)
{
    //unsigned char i;
	lcd8544_clear();	
	lcd8544_rect(0,0,82,47,1); // �������������

	lcd8544_putstr(12, 10, "���� USART", 1); // ����� ������ ������ 
	lcd8544_putstr(18, 20, "�������: ", 1); // ����� ������ ������
	lcd8544_putstr( 18, 30, rx_data, 1); // ����� ������� ������
						
	lcd8544_refresh(); // ����� ������ �� ����� ! ��� ����� ������ ����� �� ����� !
    FOD = 0; 
}

void LCD_Out_TX_completed (void)
{

	lcd8544_clear();	
	lcd8544_rect(0,0,82,47,1); // �������������

	lcd8544_putstr(26, 10, "USART", 1); // ����� ������ ������
	lcd8544_putstr( 18, 20, "��������", 1); // ����� ������ ������
	lcd8544_putstr( 16, 30, "���������", 1); // ����� ������� ������

	lcd8544_refresh(); // ����� ������ �� ����� ! ��� ����� ������ ����� �� ����� !
	
	FTC = 0;
     
}
