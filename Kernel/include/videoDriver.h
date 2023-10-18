#ifndef _VIDEO_DRIVER_H_
#define _VIDEO_DRIVER_H_

#include <stdint.h>

typedef struct {
	uint8_t b;
	uint8_t g;
	uint8_t r;
}Color;

#define BLACK (Color) {0x00, 0x00, 0x00}
#define WHITE (Color) {0xFF, 0xFF, 0xFF}
#define GREEN (Color) {0x1F, 0xED, 0x11}
#define GRAY (Color) {0x90, 0x90, 0x90}
#define BLUE (Color) {0xFF, 0x00, 0x00}
#define RED (Color) {0x00, 0x00, 0xFF}
#define MAGENTA (Color) {0x87, 0x18, 0xD6}

void * getPixel(uint16_t x, uint16_t y);
void fillrect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, Color color);
void colorScreen(Color color);
void clearScreen();
void printSquare(uint16_t x, uint16_t y, uint16_t l, Color color);
void printChar(uint16_t x, uint16_t y, uint8_t c, Color color);
void printStringAt(uint16_t x, uint8_t y, uint8_t * string, Color color);
void printString(uint8_t * string, Color color);
void printBase(uint64_t value, uint32_t base);
uint16_t getHeight();
uint16_t getWidth();
int changeFontSize(uint32_t size);

#endif
