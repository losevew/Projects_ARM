//------------------------------------------------------------------------------
// This is Open source software. You can place this code on your site, but don't
// forget a link to my YouTube-channel: https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// Это программное обеспечение распространяется свободно. Вы можете размещать
// его на вашем сайте, но не забудьте указать ссылку на мой YouTube-канал 
// "Электроника в объектике" https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// Автор: Надыршин Руслан / Nadyrshin Ruslan
//------------------------------------------------------------------------------
#ifndef _SCROLL_DEMO_H
#define _SCROLL_DEMO_H

#include "stm32f0xx.h"
#include "stdint.h"

// Процедура прокручивает на матрице буфер pBuff (ВЛЕВО/ВПРАВО)
void demo_ScrollBuff(uint8_t *pBuff, uint16_t ScrollLines, uint8_t RightToLeft);

#endif
