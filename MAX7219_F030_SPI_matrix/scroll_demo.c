//------------------------------------------------------------------------------
// This is Open source software. You can place this code on your site, but don't
// forget a link to my YouTube-channel: https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// Это программное обеспечение распространяется свободно. Вы можете размещать
// его на вашем сайте, но не забудьте указать ссылку на мой YouTube-канал 
// "Электроника в объектике" https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// Автор: Надыршин Руслан / Nadyrshin Ruslan
//------------------------------------------------------------------------------
#include "scroll_demo.h"
#include "max7219\max7219.h"
#include "max7219\ledmatrix.h"


//==============================================================================
// Процедура прокручивает на матрице буфер pBuff (ВЛЕВО/ВПРАВО)
//==============================================================================
void demo_ScrollBuff(uint8_t *pBuff, uint16_t ScrollLines, uint8_t RightToLeft)
{
  uint16_t ScrollIdx = (RightToLeft) ? 0 : ScrollLines - 1;

  ledmatrix_fill_screenbuff(0x00);  // Очистка буфера кадра
  
  for (uint16_t i = 0; i < ScrollLines + (MAX7219_NUM * 8); i++)
  {
    if (RightToLeft)    // Прокрутка справа налево
    {
      ledmatrix_ScrollLeft();

      if (i < ScrollLines)
        ledmatrix_screenbuff[(MAX7219_NUM * 8) - 1] = pBuff[ScrollIdx++];
      else
        ledmatrix_screenbuff[(MAX7219_NUM * 8) - 1] = 0;
    }
    else                // Прокрутка слева направо
    {
      ledmatrix_ScrollRight();

      if (i < ScrollLines)
        ledmatrix_screenbuff[0] = pBuff[ScrollIdx--];
      else
        ledmatrix_screenbuff[0] = 0;
    }
      
    ledmatrix_update_from_buff();
      
    delay_ms(50);
  }
}
//==============================================================================
