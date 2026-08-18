/* https://github.com/Romshh */

#ifndef LCD_CONF_HEADER
#define LCD_CONF_HEADER

#include "main.h"

#define LCD_SPI            hspi1

#define LCD_SPI_TIMEOUT    1000

#define LCD_CS_PORT        GPIOA
#define LCD_CS_PIN         GPIO_PIN_4

#define LCD_DC_PORT        GPIOA
#define LCD_DC_PIN         GPIO_PIN_6

#define LCD_RST_PORT       GPIOB
#define LCD_RST_PIN        GPIO_PIN_6

#define LCD_ORIENTATION    0x28
#define LCD_W              320
#define LCD_H              240

#define LCD_TEXT_SCALE_MAX 4

#endif