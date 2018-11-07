//------------------------------------------------------------------------------
// This is Open source software. You can place this code on your site, but don't
// forget a link to my YouTube-channel: https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// Это программное обеспечение распространяется свободно. Вы можете размещать
// его на вашем сайте, но не забудьте указать ссылку на мой YouTube-канал 
// "Электроника в объектике" https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// Автор: Надыршин Руслан / Nadyrshin Ruslan
//------------------------------------------------------------------------------
#ifndef _MAX7219_H
#define _MAX7219_H

#include "stdint.h"
#include "stm32f0xx.h"

/* пины порта к которым подключен MAX7219

DIN - A7
CS  - A4
CLK - A5

*/


// Кол-во MAX7219 в цепочке
#define MAX7219_NUM             4

// Расположение пина управления сигналом CS у max7219
#define MAX7219_PORT   GPIOA

#define CS             GPIO_ODR_4
#define CS_PIN_OTYP    GPIO_OTYPER_OT_4
#define CS_PIN_OSPEED  GPIO_OSPEEDR_OSPEEDR4
#define CS_PIN_MODE    GPIO_MODER_MODER4_0

// Управление линией MAX7219_CS
#define MAX7219_CS1  MAX7219_PORT->ODR |= CS
#define MAX7219_CS0  MAX7219_PORT->ODR &= ~CS


// Коды команд MAX7219
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


// Варианты декодирования цифр для команды DECODE MODE (используется с 7-сегментными индикаторами)
#define MAX7219_NO_DECODE       0x00
#define MAX7219_0_FOR_71        0x01
#define MAX7219_30_FOR_74       0x0F
#define MAX7219_DECODE_FOR_70   0xFF


// Индекс микросхемы max7219, указывающий что команду+данные нужно записать во все микросхемы цепочки
#define MAX7219_ALL_IDX         0xFF

#define hibyte(a) ((a>>8) & 0xFF)
#define lobyte(a) ((a) & 0xFF)

// Процедура задержек
void delay_ms(__IO uint32_t nTime);

// Процедура инициализирует интерфейс обмена с цепочкой max7219
void max7219_init(void);
// Процедура отправляет команду с данным в один или во все max7219 в цепочке
void max7219_send(uint8_t MAX_Idx, uint8_t Cmd, uint8_t Data);
// Процедура отправляет массив команд в max7219
void max7219_sendarray(uint16_t *pArray);
// Процедура отправляет 16-битное слово по SPI
void max7219_send16bit(uint16_t Word);
// Процедура устанавливает режим декодирования символов в 1 или во всех max7219
void max7219_set_decodemode(uint8_t MAX_Idx, uint8_t DecodeMode);
// Процедура устанавливает яркость в 1 или во всех max7219
void max7219_set_intensity(uint8_t MAX_Idx, uint8_t Intensity);
// Процедура устанавливает кол-во знаков/строк в 1 или во всех max7219
void max7219_set_scan_limit(uint8_t MAX_Idx, uint8_t Limit);
// Процедура включает/выключает max7219. После подачи питания он выключен
void max7219_set_run_onoff(uint8_t MAX_Idx, uint8_t On);
// Процедура включает/выключает тестовый режим max7219 (горят все индикаторы)
void max7219_set_testmode_onoff(uint8_t MAX_Idx, uint8_t On);

#endif



