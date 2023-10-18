#include <videoDriver.h>
#include <stdint.h>
#include <exceptions.h>
#include <syscallManager.h>


void endException(uint8_t * str);
void printSavedRegisters();

extern uint64_t registers[REGISTER_NUM];

void exceptionDispatcher(uint64_t e){
    switch (e) {
        case 0:
            divideZeroException();
            break;
        case 6:
            invalidOpcodeException();
            break;
    }
    return;
}

void endException(uint8_t * errorString){
    colorScreen(BLUE);
    printStringAt(0,0, (uint8_t *) errorString, RED);
    printString((uint8_t *) "\nYOU WILL BE RETURNED TO THE SHELL IN TEN SECONDS\n\n", GREEN);
    printSavedRegisters();
    sys_wait(10000);
    loadMain();
}

void divideZeroException(){
    clearScreen();
    endException((uint8_t *) ERROR0);
}

void invalidOpcodeException(){
    clearScreen();
    endException((uint8_t *) ERROR6);
}

void printSavedRegisters(){
    char * regNames[] = REGISTER_NAMES;
    int i = 0;
    for (; i < REGISTER_NUM - 1 ; i++){
        printString((uint8_t *) regNames[i], MAGENTA);
        printString((uint8_t *) "\t\t\t\t\t\t",MAGENTA);
        printBase(registers[i], 2);
        printString((uint8_t *) "\n",MAGENTA);
    }
}
