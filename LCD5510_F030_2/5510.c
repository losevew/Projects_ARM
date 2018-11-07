/**
 *   ******************************************************************************
 * @file    5110.c
 * @author  Aleksandr Kuzmin (theshurup@gmail.com) Shurup
 * @version V1.0.0
 * @date    20-October-2014
 * @brief   Library for NOKIA 5110 LCD (84*48 dots)
 *
 ******************************************************************************
 */

#include "stm32f0xx.h"
#include "5510.h"
#include "font.h"



void N5110_WriteByte(uint8_t dat, uint8_t mode){
	uint8_t i;
	//N5110_CS_LOW;

	if (mode == N5110_DATA) N5110_DC_HIGH; else N5110_DC_LOW;

	for (i = 0; i < 8; i++) {
		if(dat & 0x80) N5110_DIN_HIGH; else N5110_DIN_LOW;
		dat = dat << 1;
		N5110_SCK_LOW;
		N5110_SCK_HIGH;
	}

	//N5110_CS_HIGH;
}

void N5110_Init(N5110_TypeDef* lcd) {

	RCC->AHBENR |= LCD_PH ;
	  
	LCD_PORT->OTYPER &= ~(RST_PIN_OTYP | CS_PIN_OTYP | DC_PIN_OTYP | DIN_PIN_OTYP | SCK_PIN_OTYP);
	LCD_PORT->OSPEEDR |= RST_PIN_OSPEED | CS_PIN_OSPEED | DC_PIN_OSPEED | DIN_PIN_OSPEED | SCK_PIN_OSPEED;
	  
	LCD_PORT->MODER |= RST_PIN_MODE | CS_PIN_MODE | DC_PIN_MODE | DIN_PIN_OSPEED | SCK_PIN_OSPEED ;

    N5110_SCK_LOW;
	N5110_CS_LOW;
	N5110_RST_LOW;
	N5110_RST_HIGH;
	
//	N5110_RST_LOW;
//	N5110_DC_HIGH;
//	N5110_DIN_HIGH;
//	N5110_SCK_HIGH;
//	N5110_CS_HIGH;

	//lcd->delay_function(10);

	//N5110_RST_HIGH;

	//Configure LCD module
	N5110_WriteByte(0x21, N5110_COMMAND); //Extended instruction set selected
	N5110_WriteByte(0xC1, N5110_COMMAND); //Set LCD voltage (defined by experimentation...)
	N5110_WriteByte(0x13, N5110_COMMAND); //Set Bias for 1/48
	N5110_WriteByte(0x04, N5110_COMMAND); //Set temperature control (TC2)
	N5110_WriteByte(0x20, N5110_COMMAND); //Revert to standard instruction set
	N5110_Clear(lcd); //Clear display (still off)
	N5110_WriteByte(0x0C, N5110_COMMAND); //Set display on in "normal" mode (not inversed)

}

void N5110_WriteChar(N5110_TypeDef* lcd, uint8_t x, uint8_t y, uint8_t c) {
	uint8_t line;
	uint8_t ch = 0, i;
	c = c - 32;
	for (line = 0; line < 6; line++) {
		ch = font6_8[c][line];
		for (i=0; i<8; i++){
			if (ch&0x01)
				N5110_DrawPixel(lcd, x+line, y+i);
			else
				N5110_ClearPixel(lcd, x+line, y+i);
			ch >>= 1;
		}
	}
}

void N5110_WriteCharInv(N5110_TypeDef* lcd, uint8_t x, uint8_t y, uint8_t c) {
	uint8_t line;
	uint8_t ch = 0;

	c = c - 32;

	for (line = 0; line < 6; line++) {
		ch = ~font6_8[c][line];
		N5110_WriteByte(ch, N5110_DATA);

	}
}

void N5110_WriteString(N5110_TypeDef* lcd, uint8_t x, uint8_t y, uint8_t *s) {
	uint8_t ch;
	while (*s != '\0') {
		ch = *s;
		N5110_WriteChar(lcd, x, y, ch);
		x+=6;
		s++;
	}
}


void N5110_Clear(N5110_TypeDef* lcd) {
	uint8_t i, j;
	for (i = 0; i < 6; i++)
		for (j = 0; j < 84; j++)
			lcd->bmp->bmp[j][i] = 0;

	for(j=0;j<6;j++){
		for (i=0;i<84;i++){
			N5110_WriteByte(lcd->bmp->bmp[i][j], N5110_DATA);
		}
	}
}

void N5110_GotoXY(N5110_TypeDef* lcd, uint8_t X, uint8_t Y) {
	N5110_WriteByte(0x40 | Y, N5110_COMMAND);
	N5110_WriteByte(0x80 | X, N5110_COMMAND);
}

void N5110_WriteDec(N5110_TypeDef* lcd, uint8_t x, uint8_t y, uint16_t b) {

	uint8_t datas[4];

	datas[0] = b / 1000;
	b = b - datas[0] * 1000;
	datas[1] = b / 100;
	b = b - datas[1] * 100;
	datas[2] = b / 10;
	b = b - datas[2] * 10;
	datas[3] = b;

	datas[0] += 48;
	datas[1] += 48;
	datas[2] += 48;
	datas[3] += 48;

	N5110_WriteChar(lcd, x, y, datas[0]);
	N5110_WriteChar(lcd, x+6, y, datas[1]);
	N5110_WriteChar(lcd, x+12, y, datas[2]);
	N5110_WriteChar(lcd, x+18, y, datas[3]);
}

void N5110_Refresh(N5110_TypeDef* lcd){
	uint8_t i, j;
	for(j=0;j<6;j++){
		for (i=0;i<84;i++){
			N5110_WriteByte(lcd->bmp->bmp[i][j], N5110_DATA);
		}
	}
}

void N5110_DrawPixel(N5110_TypeDef* lcd, uint8_t X, uint8_t Y){
	N5110_GotoXY(lcd, X, Y>>3);
	lcd->bmp->bmp[X][Y>>3] |= 1<<(Y & 0x7);
	N5110_WriteByte(lcd->bmp->bmp[X][Y>>3], N5110_DATA);
}

void N5110_ClearPixel(N5110_TypeDef* lcd, uint8_t X, uint8_t Y){
	N5110_GotoXY(lcd, X, Y>>3);
	lcd->bmp->bmp[X][Y>>3] &= ~(1<<(Y & 0x7));
	N5110_WriteByte(lcd->bmp->bmp[X][Y>>3], N5110_DATA);
}

void N5110_DrawLine(N5110_TypeDef* lcd, uint8_t X1, uint8_t Y1, uint8_t X2, uint8_t Y2){
	uint8_t dx, dy, i, j;
	int8_t e =0;

	dx = (X2>X1) ? (X2-X1) : (X1-X2);
	dy = (Y2>Y1) ? (Y2-Y1) : (Y1-Y2);

	if (dx >= dy){
		j = Y1;
		if (X1 < X2){
			for (i=X1; i<X2; i++){
				N5110_DrawPixel(lcd, i, j);
				e = e + dy;
				if (2*e >= dx){
					j += (Y2>Y1)? 1 : -1;
					e = e - dx;
				}
			}
		} else {
			for (i=X1; i>X2; i--){
				N5110_DrawPixel(lcd, i, j);
				e = e + dy;
				if (2*e >= dx){
					j += (Y2>Y1)? 1 : -1;
					e = e - dx;
				}
			}
		}
	} else {
		i = X1;
		if (Y1<Y2){
			for (j=Y1; j<Y2; j++){
				N5110_DrawPixel(lcd, i, j);
				e = e + dx;
				if (2*e >= dy){
					i += (X2>X1)? 1 : -1;
					e = e - dy;
				}
			}
		} else {
			for (j=Y1; j>Y2; j--){
				N5110_DrawPixel(lcd, i, j);
				e = e + dx;
				if (2*e >= dy){
					i += (X2>X1)? 1 : -1;
					e = e - dy;
				}
			}
		}

	}
}

void N5110_DrawCircle(N5110_TypeDef* lcd, uint8_t X, uint8_t Y, uint8_t R){
	uint8_t x=0, y=R;
	int8_t d, e=0;

	while(y > 0){
		N5110_DrawPixel(lcd, X + x, Y + y);
		N5110_DrawPixel(lcd, X + x, Y - y);
		N5110_DrawPixel(lcd, X - x, Y + y);
		N5110_DrawPixel(lcd, X - x, Y - y);

		e = 2 * (d + y) - 1;
		if ((d<0)&&(e<=0)){
			d += 2*(++x) + 1;
			continue;
		}
		e = 2 * (d-x) - 1;
		if ((d>0)&&(e>0)){
			d += 1 - 2* (--y);
			continue;
		}
		x++;
		d += 2*(x-y);
		y--;
	}
	N5110_DrawPixel(lcd, X + x, Y);
	N5110_DrawPixel(lcd, X - x, Y);
}

void N5110_DrawRect(N5110_TypeDef* lcd, uint8_t X1, uint8_t Y1, uint8_t X2, uint8_t Y2){
	N5110_DrawLine(lcd, X1, Y1, X2, Y1);
	N5110_DrawLine(lcd, X1, Y2, X2, Y2);
	N5110_DrawLine(lcd, X1, Y1, X1, Y2);
	N5110_DrawLine(lcd, X2, Y1, X2, Y2);
}

void N5110_DrawClock(N5110_TypeDef* lcd, uint8_t X, uint8_t Y, uint8_t r, uint8_t ang){
	int8_t x,y;
	if (ang>45){
		x = -r * sin[60-ang];
		y = -r * sin[ang-45];
	} else if(ang >= 31){
		x = -r * sin[ang-30];
		y = r * sin[45-ang];
	} else if(ang >= 16){
		x = r * sin[30-ang];
		y = r * sin[ang-15];
	} else {
		x = r * sin[ang];
		y = -r * sin[15-ang];
	}
	N5110_DrawLine(lcd, X, Y, X + x, Y + y);
}
























