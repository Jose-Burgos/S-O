#ifndef _KEYBOARD_DRIVER_H_
#define _KEYBOARD_DRIVER_H_

#include <stdint.h>

void saveKey(uint8_t c);
void clearKeyboardBuffer();
uint8_t getCount();
uint32_t readBuf(char * str, uint32_t count);

#endif



