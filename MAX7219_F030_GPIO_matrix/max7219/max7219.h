//------------------------------------------------------------------------------
// This is Open source software. You can place this code on your site, but don't
// forget a link to my YouTube-channel: https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// ��� ����������� ����������� ���������������� ��������. �� ������ ���������
// ��� �� ����� �����, �� �� �������� ������� ������ �� ��� YouTube-����� 
// "����������� � ���������" https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// �����: �������� ������ / Nadyrshin Ruslan
//------------------------------------------------------------------------------
#ifndef _MAX7219_H
#define _MAX7219_H

#include "stdint.h"
#include "stm32f0xx.h"

/* ���� ����� � ������� ��������� MAX7219

DIN - A0
CS  - A1
CLK - A2

*/

// ������ ��� ���������� ����������� MAX7219

#define MAX7219_PORT GPIOA	// ���� � �������� ��������� MAX7219
#define MAX7219_PH RCC_AHBENR_GPIOAEN // ���� � ������� ���������� MAX7219

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


// ���������� ������ MAX7219_CS
#define MAX7219_DIN1   MAX7219_PORT->ODR |= DIN    
#define MAX7219_DIN0   MAX7219_PORT->ODR &= ~DIN
// ���������� ������ MAX7219_CS
#define MAX7219_CS1  MAX7219_PORT->ODR |= CS
#define MAX7219_CS0  MAX7219_PORT->ODR &= ~CS
// ���������� ������ MAX7219_CLK
#define MAX7219_CLK1   MAX7219_PORT->ODR |=  CLK
#define MAX7219_CLK0   MAX7219_PORT->ODR &= ~CLK


// ���-�� MAX7219 � �������
#define MAX7219_NUM             4


// ���� ������ MAX7219
#define MAX7219_CMD_NO_OP       0x0
#define MAX7219_CMD_DIGIT_0     0x1
#define MAX7219_CMD_DIGIT_1     0x2
#define MAX7219_CMD_DIGIT_2     0x3
#define MAX7219_CMD_DIGIT_3     0x4
#define MAX7219_CMD_DIGIT_4     0x5
#define MAX7219_CMD_DIGIT_5     0x6
#define MAX7219_CMD_DIGIT_6     0x7
#define MAX7219_CMD_DIGIT_7     0x8
#define MAX7219_CMD_DECODE_MODE 0x9
#define MAX7219_CMD_INTENSITY   0xA
#define MAX7219_CMD_SCAN_LIMIT  0xB
#define MAX7219_CMD_SHUTDOWN    0xC
#define MAX7219_CMD_DISP_TEST   0xF


// �������� ������������� ���� ��� ������� DECODE MODE (������������ � 7-����������� ������������)
#define MAX7219_NO_DECODE       0x00
#define MAX7219_0_FOR_71        0x01
#define MAX7219_30_FOR_74       0x0F
#define MAX7219_DECODE_FOR_70   0xFF


// ������ ���������� max7219, ����������� ��� �������+������ ����� �������� �� ��� ���������� �������
#define MAX7219_ALL_IDX         0xFF

#define hibyte(a) ((a>>8) & 0xFF)
#define lobyte(a) ((a) & 0xFF)

// ��������� ��������
void delay_ms(__IO uint32_t nTime);

// ��������� �������������� ��������� ������ � �������� max7219
void max7219_init(void);
// ��������� ���������� ������� � ������ � ���� ��� �� ��� max7219 � �������
void max7219_send(uint8_t MAX_Idx, uint8_t Cmd, uint8_t Data);
// ��������� ���������� ������ ������ � max7219
void max7219_sendarray(uint16_t *pArray);
// ��������� ���������� 16-������ ����� �� SPI
void max7219_send16bit(uint16_t Word);
// ��������� ������������� ����� ������������� �������� � 1 ��� �� ���� max7219
void max7219_set_decodemode(uint8_t MAX_Idx, uint8_t DecodeMode);
// ��������� ������������� ������� � 1 ��� �� ���� max7219
void max7219_set_intensity(uint8_t MAX_Idx, uint8_t Intensity);
// ��������� ������������� ���-�� ������/����� � 1 ��� �� ���� max7219
void max7219_set_scan_limit(uint8_t MAX_Idx, uint8_t Limit);
// ��������� ��������/��������� max7219. ����� ������ ������� �� ��������
void max7219_set_run_onoff(uint8_t MAX_Idx, uint8_t On);
// ��������� ��������/��������� �������� ����� max7219 (����� ��� ����������)
void max7219_set_testmode_onoff(uint8_t MAX_Idx, uint8_t On);

#endif



