#include <syscallManager.h>

int getKbdBuffer(char * buf, uint32_t count, int * pos);
extern void outb(int par1, uint8_t par2);
extern uint8_t inb(uint8_t par1);

void playSound(uint32_t frequency);
void noSound();
void beep1(uint32_t frequency);
void beep2();

extern uint64_t registers[REGISTER_NUM];


uint64_t sys_write(uint8_t fd, char *string, Color color) {
    if (fd == STDERR)
        color = RED;
    
    printString((uint8_t *)string, color);
    return 0;
}


int getKbdBuffer(char * buf, uint32_t count, int * pos){
    if (getCount()){
        *pos += readBuf(buf + *pos, count - *pos - 1);
        clearKeyboardBuffer();
        return 1;
    }
    return 0;
}

uint64_t sys_read(uint8_t fd, char * buf, uint32_t count) {
    if (fd != 0)
        return 0;
    
    int i = 0;
    int read = 0;
    clearKeyboardBuffer();
    while (i < count - 1){
        read = getKbdBuffer(buf, count, &i);
        if (read && (buf[i-1] == '\n' || buf[i-1] == 0)){
            buf[i-1] = 0;
            return i;
        }
        _hlt();
    }
    buf[i] = 0;
    return i;
}

uint64_t sys_timedRead(uint8_t fd, char * buf, uint32_t count, uint32_t millis) {
    if (fd != STDIN)
        return 0;
    
    int i = 0;
    int read = 0;
    clearKeyboardBuffer();
    buf[0] = 0;
    int initial = milliseconds_elapsed();
    while ((i < count - 1) && ((milliseconds_elapsed() - initial) < millis)){
        read = getKbdBuffer(buf, count, &i);
        if (read && (buf[i-1] == '\n' || buf[i-1] == 0)){
            buf[i-1] = 0;
            return i;
        }
        _hlt();
    }
    buf[i] = 0;
    return i;
}

uint64_t sys_time() {
    return getTime();
}

uint64_t sys_date() {
    return getDate();
}

uint64_t sys_clearScreen() {
    clearScreen();
    return 0;
}

uint64_t sys_drawRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, Color color) {
    fillrect((uint16_t) x, (uint16_t) y, (uint16_t) width, (uint16_t) height, color);
    return 0;
}


void playSound(uint32_t frequency) {
 	uint32_t Div;
 	uint8_t tmp;
 
    //Set the PIT to the desired frequency
 	Div = 1193180 / frequency;
 	outb(0x43, 0xb6);
 	outb(0x42, (uint8_t) (Div) );
 	outb(0x42, (uint8_t) (Div >> 8));
 
    //And play the sound using the PC speaker
 	tmp = inb(0x61);
  	if (tmp != (tmp | 3))
 		outb(0x61, tmp | 3);
 }
 
 void noSound() {
 	uint8_t tmp = inb(0x61) & 0xFC;
 	outb(0x61, tmp);
 }
 
 void beep1(uint32_t frequency) {
 	playSound(frequency);
    sys_wait(200);
 	noSound();
 }

  void beep2() {
 	playSound(1193180);
    sys_wait(200);
 	noSound();
 }

uint64_t sys_beep(uint32_t frequency) {
    beep1(frequency);
 	sys_wait(100);
    beep2();
    return 0;
}

uint64_t sys_widthScr() {
    return getWidth();
}

uint64_t sys_heightScr() {
    return getHeight(); 
}

uint64_t sys_writeAt(uint16_t x, uint16_t y, char *string, Color color) {
    printStringAt(x, y, (uint8_t *)string, color);
    return 0;
}

uint64_t sys_wait(uint32_t millis){
    int initial = milliseconds_elapsed();
    while (milliseconds_elapsed() - initial < millis)
        _hlt();
    
    return 0;
}


uint64_t sys_inforeg(uint64_t array[REGISTER_NUM]){
    for (int i = 0 ; i < REGISTER_NUM ; i++)
        array[i] = registers[i];
    return 1;
}


uint64_t sys_changeFontSize(uint32_t size) {
    return changeFontSize(size);
}

void * sys_malloc(uint64_t memSize) {
    return allocMemory(memSize);
}

