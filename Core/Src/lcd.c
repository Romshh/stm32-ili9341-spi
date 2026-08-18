/* https://github.com/Romshh */

#include "lcd.h"
#include "main.h"
#include "font5x7.h"

extern SPI_HandleTypeDef LCD_SPI;

#define SWRESET 0x01
#define SLPOUT 0x11
#define COLMOD 0x3A
#define DISPON 0x29
#define MADCTL 0x36
#define CASET 0x2A
#define PASET 0x2B
#define RAMWR 0x2C

static void lcd_cmd(uint8_t cmd){
	HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&LCD_SPI, &cmd, 1, LCD_SPI_TIMEOUT);
	HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_SET);
}

static void lcd_data(uint8_t data){
	HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&LCD_SPI, &data, 1, LCD_SPI_TIMEOUT);
	HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_SET);
}

void lcd_init(void){
	HAL_GPIO_WritePin(LCD_RST_PORT, LCD_RST_PIN, GPIO_PIN_RESET);
	HAL_Delay(20);
	HAL_GPIO_WritePin(LCD_RST_PORT, LCD_RST_PIN, GPIO_PIN_SET);
	HAL_Delay(120);
	lcd_cmd(SWRESET);
	HAL_Delay(150);
	lcd_cmd(SLPOUT);
	HAL_Delay(120);
	lcd_cmd(MADCTL);
	lcd_data(LCD_ORIENTATION);
	lcd_cmd(COLMOD);
	lcd_data(0x55);
	lcd_cmd(DISPON);
}

static void lcd_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1){
	lcd_cmd(CASET);
	lcd_data(x0 >> 8);
	lcd_data(x0 & 0xFF);
	lcd_data(x1 >> 8);
	lcd_data(x1 & 0xFF);
	lcd_cmd(PASET);
	lcd_data(y0 >> 8);
	lcd_data(y0 & 0xFF);
	lcd_data(y1 >> 8);
	lcd_data(y1 & 0xFF);
	lcd_cmd(RAMWR);
}

void lcd_rect_fill(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color){

	static uint8_t row[LCD_W * 2];
	for (uint16_t i = 0; i < w; i++) {
		row[2*i]     = color >> 8;
		row[2*i + 1] = color & 0xFF;
	}
	lcd_window(x, y, x + w - 1, y + h - 1);
	HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_RESET);

	for(uint16_t i = 0;i < h; i++){
		HAL_SPI_Transmit(&LCD_SPI, row, w * 2, LCD_SPI_TIMEOUT);
	}
	HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_SET);
}

void lcd_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color){
	lcd_rect_fill(x, y, w ,1, color);
	lcd_rect_fill(x, y + h-1, w, 1, color);
	lcd_rect_fill(x, y, 1, h, color);
	lcd_rect_fill(x + w-1, y, 1, h, color);
}

void lcd_clear(uint16_t color){
	lcd_rect_fill(0, 0, LCD_W, LCD_H, color);
}

void lcd_pixel(uint16_t x, uint16_t y, uint16_t color){
	lcd_rect_fill(x,y,1,1,color);
}

void lcd_image(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t *data){
	static uint8_t row[LCD_W * 2];

	lcd_window(x, y, x + w - 1, y + h - 1);
	HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_RESET);

	for (uint16_t line = 0; line < h; line++) {
		for (uint16_t col = 0; col < w; col++) {
			uint16_t pixel = data[(line * w) + col];
			row[2*col]     = pixel >> 8;
			row[2*col + 1] = pixel & 0xFF;
		}

		HAL_SPI_Transmit(&LCD_SPI, row, w * 2, LCD_SPI_TIMEOUT);
	}

	HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_SET);
}

void lcd_char(uint16_t x, uint16_t y, char c, uint16_t fg, uint16_t bg, uint8_t scale){
	uint16_t color;
	if(c < 32 || c > 126){
		c = ' ';
	}

	if(scale < 1){
		scale = 1;
	}
	if(scale > LCD_TEXT_SCALE_MAX){
		scale = LCD_TEXT_SCALE_MAX;
	}

	const uint8_t *pattern = font5x7[c - ' '];

	uint16_t w = 6 * scale;
	uint16_t h = 8 * scale;

	static uint16_t buffer[(6 * LCD_TEXT_SCALE_MAX) * (8 * LCD_TEXT_SCALE_MAX)];

	for (uint16_t line = 0; line < h; line++) {

		uint16_t font_line = line / scale;

		for (uint16_t col = 0; col < w; col++) {
			uint16_t font_col = col / scale;

			color = bg;

			if(font_col < 5 && font_line < 7){
				if((pattern[font_col] >> font_line) & 1){
					color = fg;
				}
			}

			buffer[line * w + col] = color;
		}
	}

	lcd_image(x, y, w, h, buffer);

}

void lcd_text(uint16_t x, uint16_t y, const char *s, uint16_t fg, uint16_t bg, uint8_t scale){
	while(*s){
			lcd_char(x,y,*s,fg,bg,scale);
			s++;
			x = x + 6 * scale;
	}
}
