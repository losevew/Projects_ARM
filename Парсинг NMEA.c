/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include "ssd1306.h"

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
uint8_t main_counter=0;
uint8_t incomingchar[1];
char incomingstring[80] = {0x20,};
char parsestring[80] = {0x20,};
uint8_t tempparameter[12] = {0x20,};
uint8_t nextparameterposition = 6;                                              //Исключаем первые 6 символов NMEA (GPGGL, или подобные), поскольку первый символ $ убирает приёмщик строки.
char str[80] = {0x20,};

char Nmea_GNZDA_Time[11] = {0x20,};
char Nmea_GNZDA_Day[11] = {0x20,};
char Nmea_GNZDA_Month[11] = {0x20,};
char Nmea_GNZDA_Year[11] = {0x20,};

char Nmea_GNVTG_Speed[11] = {0x20,};
char Nmea_GNGGA_Sat[11] = {0x20,};
char Nmea_GNGGA_GPS_Fix[11] = {0x20,};
char Nmea_GNGGA_Latitude[11] = {0x20,};
char Nmea_GNGGA_Latitude_Way[11] = {0x20,};
char Nmea_GNGGA_Longitude[11] = {0x20,};
char Nmea_GNGGA_Longitude_Way[11] = {0x20,};

uint8_t GMT_Hour_Offset = 3;                                                    //Смещение часового пояса GMT часы
uint8_t GMT_Minute_Offset = 0;                                                  //Смещение часового пояса GMT минуты

uint8_t Week_Day[3] = {0x20,};
uint8_t UTC_Hour;
uint8_t UTC_Minute;
uint8_t UTC_Second;
uint8_t UTC_Day;
uint8_t UTC_Month;
uint16_t UTC_Year;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Error_Handler(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
uint8_t Parameter_Parser(uint8_t counter){                                      //Возвращаем позицию следующего параметра после запятой
  uint8_t tempcounter=0;
  if(parsestring[counter] ==  0x2C ||                                           //Если попали в запятую вместо параметра, тоесть параметр пуст 
     parsestring[counter] ==  0x0D ||                                           //Или в символ переноса каретки
     parsestring[counter] == 0x2A){                                             //Или в "*"
    memset(tempparameter, 0x20, 12);                                            //Заполняем пробелами
  }
  else{
    while(parsestring[counter] != 0x2C &&                                       //Пока не найдём следующий символ "," 
          parsestring[counter] != 0x0D &&                                       //Или возврат каретки
          parsestring[counter] != 0x2A){                                        //Или "*", пишем в темповый буфер.
      tempparameter[tempcounter] = parsestring[counter];
      counter++;
      tempcounter++;
    }
  }
  return counter+1;                                                             //Сдвигаемся на одну позицию чтоб пропустить следующию после параметра запятую
}

void Reciver_handler(){
  if(parsestring[0] == 0x47 && parsestring[1] == 0x4E &&                        //Если строка начинается с GNZDA ($GNZDA,172809.456,12,07,2019,00,00*45)
     parsestring[2] == 0x5A && parsestring[3] == 0x44 &&
     parsestring[4] == 0x41){
      nextparameterposition = 6;                                                //Исключаем первые 6 символов NMEA (GPGGL, или подобные), поскольку первый символ $ убирает приёмщик строки.
      nextparameterposition = Parameter_Parser(nextparameterposition);          //Время
      memcpy(Nmea_GNZDA_Time, tempparameter, 11);
      memset(tempparameter, 0x20, 12);
      nextparameterposition = Parameter_Parser(nextparameterposition);          //День
      memcpy(Nmea_GNZDA_Day, tempparameter, 11);
      memset(tempparameter, 0x20, 12);
      nextparameterposition = Parameter_Parser(nextparameterposition);          //Месяц
      memcpy(Nmea_GNZDA_Month, tempparameter, 11);
      memset(tempparameter, 0x20, 12);
      nextparameterposition = Parameter_Parser(nextparameterposition);          //Год
      memcpy(Nmea_GNZDA_Year, tempparameter, 11);
      memset(tempparameter, 0x20, 12);
      
    }
  
  if(parsestring[0] == 0x47 && parsestring[1] == 0x4E &&                        //Если строка начинается с GNVTG
     parsestring[2] == 0x56 && parsestring[3] == 0x54 &&
     parsestring[4] == 0x47){
      nextparameterposition = 6;                                                //Исключаем первые 6 символов NMEA (GPGGL, или подобные), поскольку первый символ $ убирает приёмщик строки.
      nextparameterposition = Parameter_Parser(nextparameterposition);          //Направление курса в градусах
      nextparameterposition = Parameter_Parser(nextparameterposition);          //T
      nextparameterposition = Parameter_Parser(nextparameterposition);          //Магнитное склонение в градусах
      nextparameterposition = Parameter_Parser(nextparameterposition);          //M
      nextparameterposition = Parameter_Parser(nextparameterposition);          //Скорость над поверхностью (SOG) в узлах
      nextparameterposition = Parameter_Parser(nextparameterposition);          //N
      nextparameterposition = Parameter_Parser(nextparameterposition);          //Скорость над поверхностью (SOG) в км/ч
      memcpy(Nmea_GNVTG_Speed, tempparameter, 11);
      memset(tempparameter, 0x20, 12);
  }
  
  if(parsestring[0] == 0x47 && parsestring[1] == 0x4E &&                        //Если строка начинается с GNGGA
     parsestring[2] == 0x47 && parsestring[3] == 0x47 &&
     parsestring[4] == 0x41){
      nextparameterposition = 6;                                                //Исключаем первые 6 символов NMEA (GPGGL, или подобные), поскольку первый символ $ убирает приёмщик строки.
      nextparameterposition = Parameter_Parser(nextparameterposition);          //Время UTC
      nextparameterposition = Parameter_Parser(nextparameterposition);          //Широта (Latitude)
      memcpy(Nmea_GNGGA_Latitude, tempparameter, 11);
      memset(tempparameter, 0x20, 12);
      nextparameterposition = Parameter_Parser(nextparameterposition);          //N или S
      memcpy(Nmea_GNGGA_Latitude_Way, tempparameter, 11);
      memset(tempparameter, 0x20, 12);
      nextparameterposition = Parameter_Parser(nextparameterposition);          //Долгота (Longitude)
      memcpy(Nmea_GNGGA_Longitude, tempparameter, 11);
      memset(tempparameter, 0x20, 12);
      nextparameterposition = Parameter_Parser(nextparameterposition);          //E или W
      memcpy(Nmea_GNGGA_Longitude_Way, tempparameter, 11);
      memset(tempparameter, 0x20, 12);
      nextparameterposition = Parameter_Parser(nextparameterposition);          //Fix quality: 0–invalid, 1–GPS fix, 2–DGPS fix, 3–PPS fix, 4–Real Time Kinematic, 5–Float RTK, 6–использование данных инерциальных систем, 7–Manual input mode, 8–Simulation mode
      memcpy(Nmea_GNGGA_GPS_Fix, tempparameter, 11);
      memset(tempparameter, 0x20, 12);
      nextparameterposition = Parameter_Parser(nextparameterposition);          //количество используемых спутников
      memcpy(Nmea_GNGGA_Sat, tempparameter, 11);
      memset(tempparameter, 0x20, 12);      
      nextparameterposition = Parameter_Parser(nextparameterposition);          //геометрический фактор, HDOP 
  }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if(huart==&huart1){    
    if (incomingchar[0] == 0x24){                                               //Ищем первый символ строки "$"
        main_counter=0;
    }
    else{
      if(incomingchar[0] != 0x0A){                                              //Ищем символ переноса строки и если не найден, то заполняем массив
        incomingstring[main_counter]= incomingchar[0];
        main_counter++;
      }
      else{                                                                     //Если найден символ переноса столки, то обрабатываем данные
        memcpy(parsestring, incomingstring, 80);                                //Копируем полученый массив в массив который можно обрабатывать пока принимаются следующие данные.
        Reciver_handler();
      }
    }
  HAL_UART_Receive_IT(&huart1, incomingchar, 1);                                //Разрешаем приём 1 байта по UART
  }
}

uint8_t weekday(uint8_t wd_day, uint8_t wd_month, uint16_t wd_year){            //День недели. 0-воскресенье, 1-понедельник
  if (wd_month < 3u) {
    --wd_year;
    wd_month += 10u;
  } else
    wd_month -= 2u;
return (wd_day + 31u * wd_month / 12u + wd_year + wd_year / 4u - wd_year / 100u + wd_year / 400u) % 7u;
}

uint8_t getNumberOfDays(uint8_t Month_m, uint16_t Year_m){                      //Сколько дней в месяце
  uint8_t leap = (1 - (Year_m % 4 + 2) % (Year_m % 4 + 1)) * ((Year_m % 100 + 2) % (Year_m % 100 + 1)) + (1 - (Year_m % 400 + 2) % (Year_m % 400 + 1));
  return 28 + ((Month_m + (Month_m / 8)) % 2) + 2 % Month_m + ((1 + leap) / Month_m) + (1/Month_m) - (leap/Month_m);
}

void UTC_to_Local(){
  if(Nmea_GNZDA_Time[0] != 0x20 && Nmea_GNZDA_Day[0] != 0x20 &&                 //Проверяем все ли значения присутствуют.
     Nmea_GNZDA_Month[0] != 0x20 && Nmea_GNZDA_Year[0] != 0x20){
      uint8_t Day1 = Nmea_GNZDA_Day[0]-'0';                                     //Переводим значение дня в число
      uint8_t Day2 = Nmea_GNZDA_Day[1]-'0';
        if(Day1 == 0) UTC_Day = Day2;
        else UTC_Day = (Day1*10)+Day2;
        
      uint8_t Month1 = Nmea_GNZDA_Month[0]-'0';                                 //Переводим значение месяца в число
      uint8_t Month2 = Nmea_GNZDA_Month[1]-'0';
        if(Month1 == 0) UTC_Month = Month2;
        else UTC_Month = (Month1*10)+Month2;
        
      uint8_t Year1 = Nmea_GNZDA_Year[2]-'0';                                   //Переводим значение года в число
      uint8_t Year2 = Nmea_GNZDA_Year[3]-'0';
        if(Year1 == 0) UTC_Year = 2000 + Year2;
        else UTC_Year = 2000 + Year1*10 + Year2;
       
      uint8_t Hour1 = Nmea_GNZDA_Time[0]-'0';                                   //Переводим значение часа в число
      uint8_t Hour2 = Nmea_GNZDA_Time[1]-'0';
        if(Hour1 == 0) UTC_Hour = Hour2;
        else UTC_Hour = (Hour1*10)+Hour2;
        
      uint8_t Minute1 = Nmea_GNZDA_Time[2]-'0';                                 //Переводим значение минут в число
      uint8_t Minute2 = Nmea_GNZDA_Time[3]-'0';
        if(Minute1 == 0) UTC_Minute = Minute2;
        else UTC_Minute = (Minute1*10)+Minute2;
      if(UTC_Minute+GMT_Minute_Offset > 59){                                    //Если при смещении GMT минуты привышают 59
        UTC_Minute = (UTC_Minute+GMT_Minute_Offset) - 60;
        UTC_Hour = UTC_Hour + 1;                                                //Добавляем к текущему времени час
      }
      else UTC_Minute = UTC_Minute+GMT_Minute_Offset;
        
      uint8_t Second1 = Nmea_GNZDA_Time[4]-'0';                                 //Переводим значение секунд в число
      uint8_t Second2 = Nmea_GNZDA_Time[5]-'0';
        if(Second1 == 0) UTC_Second = Second2;
        else UTC_Second = (Second1*10)+Second2;
        
      if(UTC_Hour+GMT_Hour_Offset > 23){                                        //Если после корректировки времени часовым поясом время стало больше 24 часов
          UTC_Hour = (UTC_Hour+GMT_Hour_Offset) - 24;                           //То из полученого времени вычитаем 24
          UTC_Day = UTC_Day + 1;                                                //И прибавляем один день
          uint8_t Month_Day = getNumberOfDays(UTC_Month, UTC_Year);
          
          if(UTC_Day > Month_Day){
            UTC_Day = UTC_Day - Month_Day;
            UTC_Month = UTC_Month + 1;
            if(UTC_Month>12){
              UTC_Month = 1;
              UTC_Year = UTC_Year +1;
            }
          }
      }
      else UTC_Hour = UTC_Hour+GMT_Hour_Offset;
      
  uint8_t wd = weekday(UTC_Day, UTC_Month, UTC_Year);                           //Вычисляем день недели
    switch(wd){
      case 0: Week_Day[0] = 0x53; Week_Day[1] = 0x55; Week_Day[2] = 0x4E; break;//SUN
      case 1: Week_Day[0] = 0x4D; Week_Day[1] = 0x4F; Week_Day[2] = 0x4E; break;//MON
      case 2: Week_Day[0] = 0x54; Week_Day[1] = 0x55; Week_Day[2] = 0x45; break;//TUE
      case 3: Week_Day[0] = 0x57; Week_Day[1] = 0x45; Week_Day[2] = 0x44; break;//WED
      case 4: Week_Day[0] = 0x54; Week_Day[1] = 0x48; Week_Day[2] = 0x55; break;//THU
      case 5: Week_Day[0] = 0x46; Week_Day[1] = 0x52; Week_Day[2] = 0x49; break;//FRI
      case 6: Week_Day[0] = 0x53; Week_Day[1] = 0x41; Week_Day[2] = 0x54; break;//SAT
    }
  }
}
/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();

  /* USER CODE BEGIN 2 */
  SSD1306_Init();
  HAL_UART_Transmit(&huart1, (uint8_t*)"$PCAS03,1,0,0,0,0,1,1,0,0,0,,,0,0*03\r\n", 38,10);      //Отключаем лишнии посылки, оставляем только GNGGA, GNVTG, GNZDA
  HAL_UART_Receive_IT(&huart1, incomingchar, 1);                                //Разрешаем приём 1 байта по UART
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    UTC_to_Local();
      SSD1306_GotoXY(0,0);
      sprintf(str,"%.2u:%.2u:%.2u  Sat:%.2sF%.1s", UTC_Hour, UTC_Minute , UTC_Second, Nmea_GNGGA_Sat, Nmea_GNGGA_GPS_Fix);
      SSD1306_Puts(str, &Font_7x10, SSD1306_COLOR_WHITE);
      
      SSD1306_GotoXY(0,12);
      sprintf(str,"%.2u.%.2u.%.4u %s", UTC_Day, UTC_Month, UTC_Year, Week_Day);
      SSD1306_Puts(str, &Font_7x10, SSD1306_COLOR_WHITE);
      
      SSD1306_GotoXY(0,24);
      sprintf(str,"Speed: %.6skm/h", Nmea_GNVTG_Speed);
      SSD1306_Puts(str, &Font_7x10, SSD1306_COLOR_WHITE);
      
      SSD1306_GotoXY(0,36);
      sprintf(str,"Lat: %.11s%.1s", Nmea_GNGGA_Latitude, Nmea_GNGGA_Latitude_Way);
      SSD1306_Puts(str, &Font_7x10, SSD1306_COLOR_WHITE);
      
      SSD1306_GotoXY(0,48);
      sprintf(str,"Long: %.11s%.1s", Nmea_GNGGA_Longitude, Nmea_GNGGA_Longitude_Way);
      SSD1306_Puts(str, &Font_7x10, SSD1306_COLOR_WHITE);
      
      SSD1306_UpdateScreen();
      HAL_Delay(100);
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}
