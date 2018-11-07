/**
 *   ******************************************************************************
 * @file    5110.h
 * @author  Aleksandr Kuzmin (theshurup@gmail.com) Shurup
 * @version V1.0.0
 * @date    20-October-2014
 * @brief   Header file for NOKIA 5110 LCD library
 *
 ******************************************************************************
 */

#ifndef __N5110_H
#define __N5110_H





#define	N5110_COMMAND (uint8_t)0x00
#define N5110_DATA (uint8_t)0x01

/* пины порта к которым подключен дисплей

RST - A0
CS  - A1
DC  - A2
DIN - A3
SCK - A5

*/

#define LCD_PORT GPIOA	// порт к которому подключен дисплей


//#define N5110_CS_LOW			LCD_PORT->BSRR  = GPIO_BSRR_BR_1
//#define N5110_CS_HIGH			LCD_PORT->BSRR = GPIO_BSRR_BS_1
//#define N5110_DC_LOW			LCD_PORT->BSRR  = GPIO_BSRR_BR_2
//#define N5110_DC_HIGH			LCD_PORT->BSRR = GPIO_BSRR_BS_2
//#define N5110_DIN_LOW			LCD_PORT->BSRR  = GPIO_BSRR_BR_3
//#define N5110_DIN_HIGH			LCD_PORT->BSRR = GPIO_BSRR_BS_3
//#define N5110_RST_LOW			LCD_PORT->BSRR  = GPIO_BSRR_BR_0
//#define N5110_RST_HIGH			LCD_PORT->BSRR = GPIO_BSRR_BS_0
//#define N5110_SCK_LOW			LCD_PORT->BSRR  = GPIO_BSRR_BR_5
//#define N5110_SCK_HIGH			LCD_PORT->BSRR = GPIO_BSRR_BS_5

#define N5110_CS_LOW			LCD_PORT->ODR &= ~GPIO_ODR_1 
#define N5110_CS_HIGH			LCD_PORT->ODR |= GPIO_ODR_1 
#define N5110_DC_LOW			LCD_PORT->ODR &= ~GPIO_ODR_2
#define N5110_DC_HIGH			LCD_PORT->ODR |= GPIO_ODR_2
#define N5110_DIN_LOW			LCD_PORT->ODR &= ~GPIO_ODR_3
#define N5110_DIN_HIGH			LCD_PORT->ODR |= GPIO_ODR_3
#define N5110_RST_LOW			LCD_PORT->ODR &= ~GPIO_ODR_0
#define N5110_RST_HIGH			LCD_PORT->ODR |= GPIO_ODR_0
#define N5110_SCK_LOW			LCD_PORT->ODR &= ~GPIO_ODR_5
#define N5110_SCK_HIGH			LCD_PORT->ODR |= GPIO_ODR_5


#define RST_PIN_OTYP GPIO_OTYPER_OT_0
#define CS_PIN_OTYP GPIO_OTYPER_OT_1
#define DC_PIN_OTYP GPIO_OTYPER_OT_2
#define DIN_PIN_OTYP GPIO_OTYPER_OT_3
#define SCK_PIN_OTYP GPIO_OTYPER_OT_5

#define RST_PIN_OSPEED GPIO_OSPEEDR_OSPEEDR0
#define CS_PIN_OSPEED GPIO_OSPEEDR_OSPEEDR1
#define DC_PIN_OSPEED GPIO_OSPEEDR_OSPEEDR2
#define DIN_PIN_OSPEED GPIO_OSPEEDR_OSPEEDR3
#define SCK_PIN_OSPEED GPIO_OSPEEDR_OSPEEDR5

#define RST_PIN_MODE GPIO_MODER_MODER0_0
#define CS_PIN_MODE GPIO_MODER_MODER1_0
#define DC_PIN_MODE GPIO_MODER_MODER2_0
#define DIN_PIN_MODE GPIO_MODER_MODER3_0
#define SCK_PIN_MODE GPIO_MODER_MODER5_0

#define LCD_PH RCC_AHBENR_GPIOAEN // шина к которой подключена дисплей

static float sin[16] = {0.0,0.105,0.208,0.309,0.407,0.5,0.588,0.67,0.743,0.809,0.866,0.914,0.951,0.978,0.995,1.0};

typedef void (*N5110_Delay_Fn)(uint32_t us);


typedef struct {
	uint8_t bmp[84][6];
} N5110_Bitmap_TypeDef;

typedef struct {
	N5110_Delay_Fn		delay_function;
	N5110_Bitmap_TypeDef* bmp;
} N5110_TypeDef;


/* User API functions */
void N5110_Init(N5110_TypeDef* lcd);
void N5110_WriteByte(uint8_t dat, uint8_t mode);
void N5110_WriteChar(N5110_TypeDef* lcd, uint8_t x, uint8_t y, uint8_t c);
void N5110_WriteCharInv(N5110_TypeDef* lcd, uint8_t x, uint8_t y, uint8_t c);
void N5110_Clear(N5110_TypeDef* lcd);
void N5110_GotoXY(N5110_TypeDef* lcd, uint8_t X, uint8_t Y);
void N5110_WriteString(N5110_TypeDef* lcd, uint8_t x, uint8_t y, uint8_t *s);
void N5110_WriteDec(N5110_TypeDef* lcd, uint8_t x, uint8_t y, uint16_t buffer);
void N5110_Refresh(N5110_TypeDef* lcd);
void N5110_DrawPixel(N5110_TypeDef* lcd, uint8_t X, uint8_t Y);
void N5110_ClearPixel(N5110_TypeDef* lcd, uint8_t X, uint8_t Y);
void N5110_DrawLine(N5110_TypeDef* lcd, uint8_t X1, uint8_t Y1, uint8_t X2, uint8_t Y2);
void N5110_DrawCircle(N5110_TypeDef* lcd, uint8_t X, uint8_t Y, uint8_t R);
void N5110_DrawRect(N5110_TypeDef* lcd, uint8_t X1, uint8_t Y1, uint8_t X2, uint8_t Y2);
void N5110_DrawClock(N5110_TypeDef* lcd, uint8_t X, uint8_t Y, uint8_t r, uint8_t ang);


#endif
