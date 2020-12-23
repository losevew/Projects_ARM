
// USART Receiver buffer
#define RX_BUFFER_SIZE 8
unsigned char rx_data[RX_BUFFER_SIZE];
unsigned char rx_count=0;




//------------------------------------------------------------
// Функция обработки прерывания от USART1
//------------------------------------------------------------

void USART1_IRQHandler(void)
{
  //uint8_t chartoreceive = 0;
  
  if((USART1->ISR & USART_ISR_RXNE) == USART_ISR_RXNE)
  {
    rx_data[rx_count] = (uint8_t)(USART1->RDR); /* Прием данных, очистка флага */
	rx_count++;
	if(rx_count==RX_BUFFER_SIZE)
	{
		rx_count=0;
	}
          
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