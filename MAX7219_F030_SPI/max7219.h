
//-------------------------------------------------<                axlib v1.1                 >----------------------------------------------------
//-------------------------------------------------<   Функции управления микросхемой MAX7219  >----------------------------------------------------
//-------------------------------------------------< Кузнецов Алексей 2016 http://www.avrki.ru >----------------------------------------------------


#ifndef MAX7219_H_
#define MAX7219_H_


//-------------------------------------------------------------------------
//							Подключаемые библиотеки
//-------------------------------------------------------------------------



#include "stm32f0xx.h"

/* пины порта к которым подключен MAX7219

DIN - A7
CS  - A4
CLK - A5

*/



//-------------------------------------------------------------------------
//						Объявление служебных псевдонимов
//-------------------------------------------------------------------------

#define MAX7219_REG_DECODE		0x0900
#define MAX7219_REG_LIGHT		0x0A00
#define MAX7219_REG_NDIG		0x0B00
#define MAX7219_REG_SLEEP		0x0C00
#define MAX7219_REG_TEST		0x0F00

#define MAX7219_DECODE			0x0001
#define MAX7219_UNDECODE		0x0000
#define MAX7219_TEST_ON			0x01
#define MAX7219_TEST_OFF		0x00
#define MAX7219_POINT_ON		0x80
#define MAX7219_POINT_OFF		0x7F
#define MAX7219_POINT_NO		0x00



#define SBYTE	signed char
#define BYTE	char
#define UBYTE	unsigned char
#define WORD	int
#define UWORD	unsigned int
#define DWORD	long
#define ADATA	float

#define FALSE	0
#define TRUE	(!FALSE)

#define hibyte(a) ((a>>8) & 0xFF)
#define lobyte(a) ((a) & 0xFF)

void delay_ms(__IO uint32_t nTime);

//-------------------------------------------------------------------------
//							Инициализация переменных
//-------------------------------------------------------------------------


//-------------------------------------------------------------------------
//						Функция настройки портов и пинов
//-------------------------------------------------------------------------


void max7219_init_port(void)
{
	
	
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN ;
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN ;
	
	 /* (1) Select AF mode (10) on PA4, PA5, PA6, PA7 */
  /* (2) AF0 for SPI1 signals */
  GPIOA->MODER = (GPIOA->MODER &~(GPIO_MODER_MODER4 | GPIO_MODER_MODER5 | GPIO_MODER_MODER6 | GPIO_MODER_MODER7))\
                  | (GPIO_MODER_MODER4_1 | GPIO_MODER_MODER5_1 | GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1); /* (1) */
  GPIOA->AFR[0] = (GPIOA->AFR[0] &~(GPIO_AFRL_AFRL4 | GPIO_AFRL_AFRL5 | GPIO_AFRL_AFRL6 | GPIO_AFRL_AFRL7)); /* (2) */
	

	/* (1) Master selection, BR: Fpclk/128  CPOL and CPHA at zero (rising first edge) */
	/* (2) Slave select output enabled, NSSP enabled, RXNE IT, 8-bit Rx fifo */
	/* (3) Enable SPI1 */
	SPI1->CR1 = SPI_CR1_MSTR | SPI_CR1_BR_2 | SPI_CR1_BR_1; /* (1) */
	SPI1->CR2 = SPI_CR2_SSOE | SPI_CR2_NSSP | SPI_CR2_FRXTH | SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0; /* (2) */
	SPI1->CR1 |= SPI_CR1_SPE; /* (3) */
	
}


//-------------------------------------------------------------------------
//						Функция передачи слова
//-------------------------------------------------------------------------

void max7219_out_word(WORD data)
{
	
	if ((SPI1->SR & SPI_SR_TXE) == SPI_SR_TXE) /* Test Tx empty */
		{
			/* Will inititiate 8-bit transmission if TXE */
			*(uint8_t *)&(SPI1->DR) = hibyte(data);
			while (!(SPI1->SR & SPI_SR_RXNE));
			(void)SPI1->DR;
			*(uint8_t *)&(SPI1->DR) = lobyte(data);
			while (!(SPI1->SR & SPI_SR_RXNE));
			(void)SPI1->DR;
			//SPI1->SR |= SPI_SR_TXE;
		}
	
}

//-------------------------------------------------------------------------
//						Функция тест
//-------------------------------------------------------------------------

void max7219_test(void)
{
	max7219_out_word(MAX7219_REG_TEST | 0x0001);
	delay_ms(1000);
	max7219_out_word(MAX7219_REG_TEST & 0xFFFE);
}

//-------------------------------------------------------------------------
//						Функция инициализации микросхемы
//
//	Принимает аргументы
//						BYTE test - Включает/Выключает тест режим при инициализации
//							MAX7219_TEST_ON - Включить тест
//							MAX7219_TEST_OFF - Выключить тест
//
//						BYTE decod - Кодировать/не кодировать
//							MAX7219_DECODE - Кодировать
//							MAX7219_UNDECODE - Не кодировать
//
//						BYTE diglimit - Количество знаков от 1 до 8
//-------------------------------------------------------------------------

void max7219_init(BYTE test, BYTE decode, BYTE diglimit)
{	
	BYTE i = 0;
	BYTE code = 0;
	
	if(diglimit <= 0) diglimit = 1;
	if(diglimit >= 8) diglimit = 7;
	
  max7219_init_port();
	max7219_out_word(MAX7219_REG_TEST & 0xFFFE);
	if(test == MAX7219_TEST_ON) max7219_test();
	max7219_out_word(MAX7219_REG_SLEEP | 0x0001);
	
	if(decode == MAX7219_DECODE)
	{
		while (i <= diglimit) 
		{
			code |= (1 << i);
			i++;
		}
	}
	else if(decode == MAX7219_UNDECODE)
	{
		while (i <= diglimit)
		{
			code &= ~(1 << i);
			i++;
		}
	}
	else
	{
		code = decode;
	}
	
	max7219_out_word(MAX7219_REG_DECODE | code);
	max7219_out_word(MAX7219_REG_NDIG | diglimit);
}

//-------------------------------------------------------------------------
//						Функция установки яркости свечения
//
//	Принимает аргументы
//						BYTE light - Уровень яркости от 0 до 15
//							
//-------------------------------------------------------------------------

void max7219_light(BYTE light)
{
	max7219_out_word(MAX7219_REG_LIGHT | light);
}

//-------------------------------------------------------------------------
//						Функция вывода одного разряда
//
//	Принимает аргументы
//						BYTE add - Адрес разряда от 1 до 8
//						BYTE dig - Цифра от 0 до 15
//						BYTE point - Включает/Выключает точку текущего разряда
//							MAX7219_POINT_ON - Включить точку
//							MAX7219_POINT_OFF - Выключить точку
//							MAX7219_POINT_NO - Не обрабатывать разряд D7
//
//-------------------------------------------------------------------------

void max7219_data_out(BYTE add, BYTE dig, BYTE point)
{
	WORD data = 0x00;
	
	if(add <= 0) add =1;
	if(add >= 8) add = 8;
	
	if(point == MAX7219_POINT_ON) 
	{
		dig |= MAX7219_POINT_ON;
	}
	else
	{
		dig &= MAX7219_POINT_OFF;
	}
	
	data = add;
	data = ((data << 8) | dig);
	
	max7219_out_word(data);
}

//-------------------------------------------------------------------------
//						Функция очистка дисплея
//-------------------------------------------------------------------------

void max7219_clear()
{
	BYTE i = 1;
	while(i <= 8)
	{
		max7219_data_out(i, 15, MAX7219_POINT_OFF);
		i++;
	}
}

#endif /* MAX7219_H_ */
