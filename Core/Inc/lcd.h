/* https://github.com/Romshh */

#ifndef LCD_HEADER
#define LCD_HEADER

#include "main.h"

#include "lcd_conf.h"

#define RED 0xF800
#define WHITE 0xFFFF
#define BLACK 0x0000
#define GREEN 0x07E0
#define BLUE 0x001F

void lcd_init(void);
void lcd_rect_fill(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void lcd_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void lcd_clear(uint16_t color);
void lcd_pixel(uint16_t x, uint16_t y, uint16_t color);
void lcd_image(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t *data);

void lcd_char(uint16_t x, uint16_t y, char c, uint16_t fg, uint16_t bg, uint8_t scale);
void lcd_text(uint16_t x, uint16_t y, const char *s, uint16_t fg, uint16_t bg, uint8_t scale);

#endif
