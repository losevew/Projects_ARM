/* пины порта к которым подключен дисплей

RST - A0
CE  - A1
DC  - A2
DIN - A3
CLK - A5

*/

#define RST  GPIO_ODR_0
#define SCE  GPIO_ODR_1
#define DC   GPIO_ODR_2
#define MOSI GPIO_ODR_3
#define SCLK GPIO_ODR_5

#define RST_PIN_OTYP GPIO_OTYPER_OT_0
#define SCE_PIN_OTYP GPIO_OTYPER_OT_1
#define DC_PIN_OTYP GPIO_OTYPER_OT_2
#define MOSI_PIN_OTYP GPIO_OTYPER_OT_3
#define SCLK_PIN_OTYP GPIO_OTYPER_OT_5

#define RST_PIN_OSPEED GPIO_OSPEEDR_OSPEEDR0
#define SCE_PIN_OSPEED GPIO_OSPEEDR_OSPEEDR1
#define DC_PIN_OSPEED GPIO_OSPEEDR_OSPEEDR2
#define MOSI_PIN_OSPEED GPIO_OSPEEDR_OSPEEDR3
#define SCLK_PIN_OSPEED GPIO_OSPEEDR_OSPEEDR5

#define RST_PIN_MODE GPIO_MODER_MODER0_0
#define SCE_PIN_MODE GPIO_MODER_MODER1_0
#define DC_PIN_MODE GPIO_MODER_MODER2_0
#define MOSI_PIN_MODE GPIO_MODER_MODER3_0
#define SCLK_PIN_MODE GPIO_MODER_MODER5_0


#define LCD_PORT GPIOA	// порт к которому подключен дисплей

#define LCD_PH RCC_AHBENR_GPIOAEN // шина к которой подключена дисплей

void lcd8544_senddata(unsigned char data);

void lcd8544_init(void);  // инициалиазаци€ диспле€

void lcd8544_refresh(void); // обновление экрана из буфера

void lcd8544_clear(void); // очистка диспле€

void lcd8544_command(unsigned char cmd);  // передача команд

void lcd8544_gotoxy(unsigned char x, unsigned char y); // установка курсора а произвольную позицию

void lcd8544_putpix(unsigned char x, unsigned char y, unsigned char mode); // вывод пиксела

void lcd8544_line(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char mode); // вывод линии

void lcd8544_rect(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char mode); // пр€моугольник

void lcd8544_fillrect(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char mode); // закрашенный пр€моугольник

void lcd8544_putchar(unsigned char px, unsigned char py, unsigned char ch, unsigned char mode); //  вывод символа

void lcd8544_dec(unsigned int numb, unsigned char dcount, unsigned char x, unsigned char y, unsigned char mode); // вывод дес€тичного числа

void lcd8544_putstr(unsigned char x, unsigned char y, const unsigned char str[], unsigned char mode); // вывод строки


