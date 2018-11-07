//------------------------------------------------------------------------------
// This is Open source software. You can place this code on your site, but don't
// forget a link to my YouTube-channel: https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// Это программное обеспечение распространяется свободно. Вы можете размещать
// его на вашем сайте, но не забудьте указать ссылку на мой YouTube-канал 
// "Электроника в объектике" https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// Автор: Надыршин Руслан / Nadyrshin Ruslan
//------------------------------------------------------------------------------
#ifndef _LEDMATRIX_H
#define _LEDMATRIX_H

#include "stdint.h"


extern uint8_t ledmatrix_screenbuff[];


// Процедура инициализирует матрицу
void ledmatrix_init(void);
// Процедура управляет режимом Test матрицы
void ledmatrix_testmatrix(uint8_t TestOn);
// Процедура устанавливает яркость матрицы. Диапазон 0-15
void ledmatrix_set_brightness(uint8_t Value);
// Процедура заполняет буфер кадра значением FillValue
void ledmatrix_fill_screenbuff(uint8_t FillValue);
// Процедура сдвигает содержимое буфера кадра ВЛЕВО
// Самый правый столбец сохраняет при этом своё старое значение
void ledmatrix_ScrollLeft(void);
// Процедура сдвигает содержимое буфера кадра ВПРАВО
// Самый левый столбец сохраняет при этом своё старое значение
void ledmatrix_ScrollRight(void);
// Процедура обновляет состояние индикаторов в соответствии с буфером экрана ledmatrix_screenbuff
void ledmatrix_update_from_buff(void);

#endif
