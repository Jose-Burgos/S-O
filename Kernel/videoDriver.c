// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <videoDriver.h>
#include <font.h>
#include <lib.h>

#define NEWLINE '\n'
#define BACKSPACE '\b'

#define MAX_FONTSIZE 3
#define MIN_FONTSIZE 1

uint32_t fontSize = 1;
uint32_t posX = 0;

// OSDev
struct vbe_mode_info_structure {
    uint16_t attributes;        // deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
    uint8_t window_a;           // deprecated
    uint8_t window_b;           // deprecated
    uint16_t granularity;       // deprecated; used while calculating bank numbers
    uint16_t window_size;
    uint16_t segment_a;
    uint16_t segment_b;
    uint32_t win_func_ptr;      // deprecated; used to switch banks from protected mode without returning to real mode
    uint16_t pitch;         // number of bytes per horizontal line
    uint16_t width;         // width in pixels
    uint16_t height;            // height in pixels
    uint8_t w_char;         // unused...
    uint8_t y_char;         // ...
    uint8_t planes;
    uint8_t bpp;            // bits per pixel in this mode
    uint8_t banks;          // deprecated; total number of banks in this mode
    uint8_t memory_model;
    uint8_t bank_size;      // deprecated; fontSize of a bank, almost always 64 KB but may be 16 KB...
    uint8_t image_pages;
    uint8_t reserved0;

    uint8_t red_mask;
    uint8_t red_position;
    uint8_t green_mask;
    uint8_t green_position;
    uint8_t blue_mask;
    uint8_t blue_position;
    uint8_t reserved_mask;
    uint8_t reserved_position;
    uint8_t direct_color_attributes;

    uint32_t framebuffer;       // physical address of the linear frame buffer; write here to draw to the screen
    uint32_t off_screen_mem_off;
    uint16_t off_screen_mem_size;   // fontSize of memory in the framebuffer but not being displayed on the screen
    uint8_t reserved1[206];
} __attribute__ ((packed));


static uint32_t uintToBase(uint64_t value, uint8_t *buffer, uint32_t base);
void printBase(uint64_t value, uint32_t base);

static uint8_t buffer[64] = {0}; // {'0'}

static struct vbe_mode_info_structure *screenData = (void *)0x5C00;

void printSquare(uint16_t x, uint16_t y, uint16_t l, Color color) {
    fillrect(x, y, l, l, color);
}

void printChar(uint16_t x, uint16_t y, uint8_t c, Color color) {
    if (c < FIRST_CHAR || c > LAST_CHAR)
        return;
    
    int charId = c - FIRST_CHAR;
    char l;
    int count = 0;
    for (int i = 0; i < FONT_HEIGHT * fontSize && count < 32; i += fontSize, count+=2) {
        l = font[FONT_HEIGHT * charId + count];
        for (int j = 0; j < FONT_WIDTH * fontSize; j += fontSize)  {
            if ((l & 0x01) == 0x01)
                printSquare(x + j, y + i, fontSize, color);

            l = l >> 1;
        }
    }
}


void printString(uint8_t * string, Color color){
    printStringAt(posX,screenData->y_char,string,color);
}

void printStringAtX(uint16_t x, uint8_t * string, Color color){
    printStringAt(x, screenData->y_char,string,color);
}

void printStringAt(uint16_t x, uint8_t y, uint8_t *string, Color color) {
    int i = 0;
    int j = 0;
    uint8_t c;
    while ((c = string[j++]) != 0) {
        if (((x + fontSize * (i * FONT_WIDTH + (FONT_WIDTH/2))) > screenData->width) || c == NEWLINE) {
            x = 0;
            i = 0;
            if ((y + 0x02) * (FONT_HEIGHT/2) * fontSize >= screenData->height) {
                void *dst = (void *)((uint64_t)screenData->framebuffer);
                void *src = (void *)(dst + sizeof(Color) * fontSize * (FONT_HEIGHT/2) * (uint64_t)screenData->width);
                uint64_t length = sizeof(Color) * fontSize * ((uint64_t)screenData->width * (screenData->height - (FONT_HEIGHT/2)));
                memcpy(dst,src,length);
                memset(dst + length, 0, sizeof(Color) * fontSize * (uint64_t) screenData->width * (FONT_HEIGHT/2));
            } else
                y += 1;
        }
        if (c == BACKSPACE) {
            i--;
            fillrect(x + FONT_WIDTH * i * fontSize, y * FONT_HEIGHT/2 * fontSize, x + 1 + FONT_WIDTH * i * fontSize, (y + 1) * FONT_HEIGHT/2 * fontSize, BLACK);
        } else if (c != NEWLINE) {
            printChar(x + FONT_WIDTH * i * fontSize, y * FONT_HEIGHT/2 * fontSize, c, color);
            i++;
        }
    }
    posX = x + i * FONT_WIDTH * fontSize;
    screenData->y_char = y;
}

void fillrect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, Color color) {
    if (x >= screenData->width || y >= screenData->height)
        return;
    

    uint16_t maxX = screenData->width - x;
    if (w > maxX)
        w = maxX;

    uint16_t maxY = screenData->height - y;
    if (h > maxY)
        h = maxY;

    Color *where = getPixel(x, y);
    unsigned int lineBreak = screenData->width - w;

    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++)
            *(where++) = color;

        where += lineBreak;
    }
}

void *getPixel(uint16_t x, uint16_t y) {
    return (void *)(screenData->framebuffer + sizeof(Color) * (x + (y * (uint64_t)screenData->width)));
}

void colorScreen(Color color) {
    fillrect(0, 0, screenData->width, screenData->height, color);
}

void clearScreen() {
    screenData->y_char = 0x00;
    posX = 0;
    colorScreen(BLACK);
}

void printBin(uint64_t value) {
    printBase(value, (uint32_t)2);
    printString((uint8_t *) "b",WHITE);
}

void printBase(uint64_t value, uint32_t base) {
    uintToBase(value, buffer, base);
    printString(buffer, WHITE);
}

static uint32_t uintToBase(uint64_t value, uint8_t *buffer, uint32_t base) {
    uint8_t *p = buffer;
    uint8_t *p1, *p2;
    uint32_t digits = 0;

    do {
        uint32_t remainder = value % base;
        *p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
        digits++;
    } while (value /= base);

    *p = 0;

    p1 = buffer;
    p2 = p - 1;
    while (p1 < p2) {
        char tmp = *p1;
        *p1 = *p2;
        *p2 = tmp;
        p1++;
        p2--;
    }

    return digits;
}

uint16_t getHeight() {
    return screenData->height;
}

uint16_t getWidth() {
    return screenData->width;
}

int changeFontSize(uint32_t dif) {
    if ((fontSize + dif > MAX_FONTSIZE) || (fontSize + dif < MIN_FONTSIZE))
        return 0;

    fontSize = fontSize + dif;
    clearScreen();
    return 1;
}
