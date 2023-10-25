#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <videoDriver.h>
#include <idtLoader.h>
#include <syscallManager.h>


extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;


static const uint64_t PageSize = 0x1000;

static void *const uCodeModuleAddress = (void *)0x400000;
static void *const uDataModuleAddress = (void *)0x500000;


typedef int (*EntryPoint)();

void fun1(uint64_t * num1, uint64_t * num2){
    *num2 = *num1 + 10;
}

void fun2(char * str){
    str[4] = ' ';
    str[5] = 'c';
    str[6] = 'o';
    str[7] = 'm';
    str[8] = 'o';
    str[9] = 0;
}

void testMemoryManager(){
    info_Mem info;
    getInfoMem(&info);
    printString((uint8_t *)"before all\n", RED);
    printBase(info.allocated, 10);
    printString((uint8_t *)"\n", WHITE);
    printBase(info.free, 10);
    printString((uint8_t *)"\n", WHITE);
    printBase(info.total, 10);
    printString((uint8_t *) "\n", WHITE);
    uint64_t * num1 = sys_malloc(sizeof(uint64_t) * 32);
    uint64_t * num2 = sys_malloc(sizeof(uint64_t) * 32);
    *num1 = 4;
    *num2 = 7;
    printBase(*num1, 10);
    printBase(*num2, 10);
    printBase(*num1, 10);
    printBase(*num2, 10);
    fun1(num1, num2);
    
    char * str = sys_malloc(sizeof(char) * 514);
    getInfoMem(&info);
    
    str[0] = 'H';
    str[1] = 'o';
    str[2] = 'l';
    str[3] = 'a';
    str[4] = 0;

    printString((uint8_t *)"\n", WHITE);
    printString((uint8_t *)str, WHITE);
    fun2(str);
    printString((uint8_t *)"\n", WHITE);
    printString((uint8_t *)str, BLUE);
    printString((uint8_t *)"\n", WHITE);
    printString((uint8_t *)str, BLUE);
    printString((uint8_t *)"\n", WHITE);
    printString((uint8_t *)str, BLUE);
    printString((uint8_t *)"\n", WHITE);
    printString((uint8_t *)str, BLUE);

    printString((uint8_t *)"\nbefore free\n", RED);
    printBase(info.allocated, 10);
    printString((uint8_t *)"\n", WHITE);
    printBase(info.free, 10);
    printString((uint8_t *)"\n", WHITE);
    printBase(info.total, 10);
    printString((uint8_t *)"\n", WHITE);

    free(str);
    free(num1);
    free(num2);

    getInfoMem(&info);
    printString((uint8_t *)"after free\n", RED);
    printBase(info.allocated, 10);
    printString((uint8_t *)"\n", WHITE);
    printBase(info.free, 10);
    printString((uint8_t *)"\n", WHITE);
    printBase(info.total, 10);
    printString((uint8_t *) "\n", WHITE);


    return;

}



void clearBSS(void *bssAddress, uint64_t bssSize) {
    memset(bssAddress, 0, bssSize);
}

void *getStackBase() {
    return (void *)((uint64_t)&endOfKernel + PageSize * 8 // The size of the stack itself, 32KiB
            - sizeof(uint64_t)                    // Begin at the top of the stack
            );
}

void *initializeKernelBinary() {

    void *moduleAddresses[] = {
        uCodeModuleAddress,
        uDataModuleAddress
        };

    void * endOfModules = loadModules(&endOfKernelBinary, moduleAddresses);

    clearBSS(&bss, &endOfKernel - &bss);
    void * startOfMem = (void *)(((uint8_t *) endOfModules + PageSize - (uint64_t) endOfModules % PageSize));

    initalizeMemoryManager(startOfMem, 0x100000);

    return getStackBase();
}

int main() {
    load_idt();
    clearScreen();
    testMemoryManager();

    //((EntryPoint) uCodeModuleAddress)();

    return 0;
}
