
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

DIN - A0
CS  - A1
CLK - A2

*/

// Выводы для управление микросхемой MAX7219
#define DIN  GPIO_ODR_0
#define CS   GPIO_ODR_1
#define CLK  GPIO_ODR_2

#define DIN_PIN_OTYP GPIO_OTYPER_OT_0
#define CS_PIN_OTYP GPIO_OTYPER_OT_1
#define CLK_PIN_OTYP GPIO_OTYPER_OT_2

#define DIN_PIN_OSPEED GPIO_OSPEEDR_OSPEEDR0
#define CS_PIN_OSPEED GPIO_OSPEEDR_OSPEEDR1
#define CLK_PIN_OSPEED GPIO_OSPEEDR_OSPEEDR2

#define DIN_PIN_MODE GPIO_MODER_MODER0_0
#define CS_PIN_MODE GPIO_MODER_MODER1_0
#define CLK_PIN_MODE GPIO_MODER_MODER2_0

#define MAX7219_PORT GPIOA	// порт к которому подключен MAX7219

#define MAX7219_PH RCC_AHBENR_GPIOAEN // шина к которой подключена MAX7219

// Управление линией MAX7219_CS
#define MAX7219_DIN1   MAX7219_PORT->ODR |= DIN    
#define MAX7219_DIN0   MAX7219_PORT->ODR &= ~DIN
// Управление линией MAX7219_CS
#define MAX7219_CS1  MAX7219_PORT->ODR |= CS
#define MAX7219_CS0  MAX7219_PORT->ODR &= ~CS
// Управление линией MAX7219_CLK
#define MAX7219_CLK1   MAX7219_PORT->ODR |=  CLK
#define MAX7219_CLK0   MAX7219_PORT->ODR &= ~CLK

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

//#define MAX7219_CS_LOW()		(MAX7219_CS_PORT = GPIO_BSRR_BR_MAX7219_CS_PIN))
//#define MAX7219_CS_HIGHT()		(MAX7219_CS_PORT = GPIO_BSRR_BS_MAX7219_CS_PIN)

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
	RCC->AHBENR |= MAX7219_PH ;
	  
	MAX7219_PORT->OTYPER &= ~(DIN_PIN_OTYP | CS_PIN_OTYP | CLK_PIN_OTYP );
	MAX7219_PORT->OSPEEDR |= (DIN_PIN_OSPEED | CS_PIN_OSPEED | CLK_PIN_OSPEED) ;
	MAX7219_PORT->MODER |= (DIN_PIN_MODE | CS_PIN_MODE | CLK_PIN_MODE );
	
	MAX7219_DIN0;
	MAX7219_CS1;
	MAX7219_CLK0;
	
}


//-------------------------------------------------------------------------
//						Функция передачи слова
//-------------------------------------------------------------------------

void max7219_out_word(WORD data)
{

  BYTE i = 0;
	WORD temp = 0;
	
	MAX7219_CS0;
		
	while(i<16)
	{
		temp = data;
		
		if((temp << i) & 0x8000)
		{
			MAX7219_DIN1;
		}
		else
		{
			MAX7219_DIN0;
		}
		
		MAX7219_CLK1;		
		MAX7219_CLK0;
		
		i++;
	}
	
	MAX7219_CS1;

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
