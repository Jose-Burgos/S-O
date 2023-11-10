#include <syscalls.h>
#include <color.h>
#include <timer.h>
#include <lib.h>
#include <tron.h>
#include "../tests/include/tst.h"
#include <greets.h>

#define COMMAND_CHAR "$> "
#define CURSOR "|"
#define BACKSPACE '\b'
#define MAX_KBD_BUF 55

#define SHELL_NAME "Shell"

#define HELP_COMMAND "help"
#define CLEAR_COMMAND "clear"
#define EXIT_COMMAND "exit"
#define TRON_COMMAND "tron"
#define DIVIDE_BY_ZERO "div-zero"
#define INVALID_OP "invalid-op"
#define DATE_COMMAND "date"
#define TIME_COMMAND "time"
#define DATE_TIME_COMMAND "datetime"
#define INC_FONT_SIZE_COMMAND "inc-font"
#define DEC_FONT_SIZE_COMMAND "dec-font"
#define INFOREG_COMMAND "inforeg"
#define PRINTMEM_COMMAND "printmem"
#define PRINT_HEAP_STATUS_COMMAND "mem"
#define PID_COMMAND "pid"
#define PS_COMMAND "ps"
#define LOOP_COMMAND "loop"
#define KILL_COMMAND "kill"
#define NICE_COMMAND "nice"
#define BLOCK_COMMAND "block" 
#define TEST_MM_COMMAND "test-mm"
#define TEST_PROCESSES_COMMAND "test-processes"
#define TEST_PRIORITY_COMMAND "test-priority"
#define TEST_SYNC_COMMAND "test-sync"

#define MAX_TERMINAL_CHARS 124          // 124 = (1024/8) - 4 (number of characters that fit in one line minus the command prompt and cursor characters)
#define HELP_MESSAGE "HELP:\n\
The following is a list of the different commands the shell can interpret and a short description of what they do:\n\
\
help              - Displays the available commands the shell can interpret and a short description of them\n\
clear             - Clears screen allowing more text to be written\n\
exit              - Exit the shell returning to kernel space. This command will shut down the program\n\
tron              - Launches tron game\n\
div-zero          - Calls function that showcases the Divide By Zero Exception\n\
invalid-op        - Calls function that showcases the Invalid Opcode Exception\n\
date              - Displays date\n\
time              - Displays time\n\
datetime          - Displays date and time\n\
inc-font          - Increases font size\n\
dec-font          - Decreases font size\n\
inforeg           - Displays the contents of all the registers at a given time.\n\
                    To save registers press and release the CTRL key.\n\
                    If the command is called before pressing CTRL at least once,\n\
                    the registers will appear as if they have the value 0\n\
printmem          - Receives a parameter in hexadecimal. Displays the next 32 bytes after the given memory direction given\n\
mem               - Prints the status of the heap\n\
pid               - Prints the pid of the current process\n\
ps                - Prints the status of all processes\n\
loop              - Creates a process that prints a message every 10 seconds\n\
kill              - Receives a pid as a parameter and kills the process with that pid\n\
nice              - Receives a pid and a priority as parameters and changes the priority of the process with that pid\n\
block             - Receives a pid as a parameter and blocks the process with that pid\n\
test-processes    - Tests process creation\n\
test-priority     - Tests process priority\n\
test-mm           - Tests memory manager\n\
test-sync         - Tests synchronization\n"

#define INCREASE 1
#define DECREASE -1

#define REGISTER_NUM 17
#define REGISTER_NAMES {"RIP", "RAX", "RBX", "RCX", "RDX", "RSI", "RDI", "RBP", "RSP", "R8 ", "R9 ", "R10", "R11", "R12", "R13", "R14", "R15"}

#define PRINT_BYTES 32

#define COMMAND_NOT_FOUND_MESSAGE "Command not found"
#define INCREASE_FONT_FAIL "Maximum font size reached"
#define DECREASE_FONT_FAIL "Minimum font size reached"

#define NEWLINE "\n"

void shell(int argc, char **argv);
void bufferRead(char **buf);
int readBuffer(char *buf);
void printLine(char *str);
long readDecimalInput(char * buf);
void helpCommand(void);
void printNewline(void);
void testInvalidOpException();
void testDivideByZeroException();
void testMemory(char * argv[]);
void ProcessesTest(char * argv[]);
void PrioTest();
void SyncTest(char * argv[]);

void printInforeg();
void printErrorMessage(char * program, char * errorMessage);

int increaseFontSize();
int decreaseFontSize();

extern void invalidOpcode();
extern void divideZero();

void shell(int argc, char **argv) {
    int out = 1;

    while (out) {
        char str[MAX_TERMINAL_CHARS] = {0};
        char *string = str;
        bufferRead(&string);
        printf("\b");
        printNewline();
        out = readBuffer(string);
    }
}

void bufferRead(char **buf) {
    int c = 1;
    int i = 0;
    (*buf)[i] = 0;
    printString(COMMAND_CHAR, GREEN);
    printf(CURSOR);
    while (c != 0 && i < MAX_TERMINAL_CHARS - 1) {
        c = getChar();
        if (c == BACKSPACE) {
            if (i > 0) {
                (*buf)[--i] = 0;
                printf("\b");
                printf("\b");
                printf(CURSOR);
            }
        } else if (c >= ' ') {
            (*buf)[i++] = (char) c;
            (*buf)[i] = 0;
            printf("\b");
            printf(*buf + i - 1);
            printf(CURSOR);
        }
    }
}

void printmem(char * buf) {
    int i = 0;
    while (buf[i] != 0 && buf[i] == ' ')
        i++;
    if (buf[i] == 0){
        printErrorMessage(PRINTMEM_COMMAND, "No argument received");
        printNewline();
        return ;
    }
    if (buf[i] == '0')
        i++;
    else {
        printErrorMessage(PRINTMEM_COMMAND, "Argument must be a hexadecimal value");
        printNewline();
        return ;
    }
    if (buf[i] == 'x')
        i++;
    else {
        printErrorMessage(PRINTMEM_COMMAND, "Argument must be a hexadecimal value");
        printNewline();
        return ;
    }
    if (buf[i] == 0){
        printErrorMessage(PRINTMEM_COMMAND, "Argument must be a hexadecimal value");
        printNewline();
        return ;
    }
    long long accum = 0;
    for (; buf[i] != 0 ; i++){
        if (buf[i] >= 'a' && buf[i] <= 'f')
            accum = 16*accum + buf[i] - 'a' + 10;
        else if (buf[i] >= '0' && buf[i] <= '9')
            accum = 16*accum + buf[i] - '0';
        else {
            printErrorMessage(PRINTMEM_COMMAND, "Argument must be a hexadecimal value");
            printNewline();
            return ;
        }
    }
    long long * pointer = (long long *) accum;
    if (0xFFFFFFFF - accum <= 32){
        printErrorMessage(PRINTMEM_COMMAND, "Input number is too big, limit is 0xFFFFFFDE");
        printNewline();
        return ;
    }
    for (long long j = 0 ; j < PRINT_BYTES && accum + j + 2 < 0xFFFFFFFFFFFFFFFF; j++){
        printBase((int) j,10);
        printf("d\n");
        printBase(pointer[j], 2);
        printf("b\n");
    }
}

char * itoa2(long number) {
	char * str = malloc(12);
	int digits = 1;
	for (long n = number / 10; n != 0; digits++, n /= 10);

	if (digits == 1) {
		str[0] = '0';
		str[1] = number + '0';
		str[2] = 0;
		return str;
	}

	str[digits] = 0;
	for (int i = digits - 1; i >= 0; i--) {
		str[i] = (number % 10) + '0';
		number /= 10;
	}

	return str;
}

int readBuffer(char *buf) {
    int l;
    if (!strcmp(buf, ""));
    else if (!strncmp(buf, PRINTMEM_COMMAND, l = strlen(PRINTMEM_COMMAND))){
        if (buf[l] != ' ' && buf[l] != 0){
            printErrorMessage(buf, COMMAND_NOT_FOUND_MESSAGE);
            printNewline();
            return 1;
        }
        printmem(buf + l);
    }
    else if (!strcmp(buf, HELP_COMMAND))
        helpCommand();
    else if (!strcmp(buf, CLEAR_COMMAND))
        clear();
    else if (!strcmp(buf, TRON_COMMAND)){
        clear();

        // Lower font size
        int count = 0;
        for (; decreaseFontSize() ;count++);

        mainTron();             // Call tron game

        // Reset font size to previous value
        for (int i = 0 ; i < count ; i++)
            increaseFontSize();
        clear();
    } else if (!strcmp(buf, DATE_COMMAND)){
        char str[MAX_TERMINAL_CHARS] = {0};
        char * string = str;
        getDateFormat(string);
        printf("%s\n",string);
    } else if (!strcmp(buf, TIME_COMMAND)){
        char str[MAX_TERMINAL_CHARS] = {0};
        char * string = str;
        getTimeFormat(string);
        printf("%s\n",string);
    } else if (!strcmp(buf, DATE_TIME_COMMAND)){
        char str[MAX_TERMINAL_CHARS] = {0};
        char * string = str;
        getDateAndTime(string);
        printf("%s\n",string);
    } else if (!strcmp(buf, INC_FONT_SIZE_COMMAND)){
        int check = increaseFontSize();
        if (!check){
            printErrorMessage(buf, INCREASE_FONT_FAIL);
            printNewline();
        } else
            clear();
    }
    else if (!strcmp(buf, DEC_FONT_SIZE_COMMAND)){
        int check = decreaseFontSize();
        if (!check){
            printErrorMessage(buf, DECREASE_FONT_FAIL);
            printNewline();
        } else
            clear();
    } else if (!strcmp(buf, INFOREG_COMMAND))
        printInforeg();
    else if (!strcmp(buf, DIVIDE_BY_ZERO)){
        testDivideByZeroException();
        return 0;
    } else if (!strcmp(buf, INVALID_OP)){
        testInvalidOpException();
        return 0;
    } else if (!strcmp(buf, EXIT_COMMAND)){
        clear();
        return 0;
    } else if (!strcmp(buf, PS_COMMAND)){
        ps();
    } else if (!strncmp(buf, KILL_COMMAND, l = strlen(KILL_COMMAND))){
        if (buf[l] != ' ' && buf[l] != 0){
            printErrorMessage(buf, COMMAND_NOT_FOUND_MESSAGE);
            printNewline();
            return 1;
        }
        long pid = readDecimalInput(buf + l);
        if (pid == -1)
            return 1;
        kill(pid);
    } else if (!strncmp(buf, NICE_COMMAND, l = strlen(NICE_COMMAND))){
        if (buf[l] != ' ' && buf[l] != 0){
            printErrorMessage(buf, COMMAND_NOT_FOUND_MESSAGE);
            printNewline();
            return 1;
        }
        long pid = readDecimalInput(buf + l);
        if (pid == -1)
            return 1;
        if (buf[l] == 0){
            printErrorMessage(buf, "No pid received");
            printNewline();
            return 1;
        }
        if (buf[l+2] != ' ' && buf[l+2] != 0){
            printErrorMessage(buf, COMMAND_NOT_FOUND_MESSAGE);
            printNewline();
            return 1;
        }
        long priority = readDecimalInput(buf + l + 2);
        if (priority == -1)
            return 1;
        nice(priority, pid);
    } else if (!strncmp(buf, BLOCK_COMMAND, l = strlen(BLOCK_COMMAND))){
        if (buf[l] != ' ' && buf[l] != 0){
            printErrorMessage(buf, COMMAND_NOT_FOUND_MESSAGE);
            printNewline();
            return 1;
        }
        long pid = readDecimalInput(buf + l);
        if (pid == -1)
            return 1;
        block(pid);
    } else if (!strcmp(buf, LOOP_COMMAND)){
        char *argv[] = {NULL};
        int pid;
        if ((pid = exec("loop", argv, &greets, 0, 0)) == -1) {
            printErrorMessage(buf, "Error creating process"); // FIX SCHEDULER
        }
    } else if (!strncmp(buf, TEST_PROCESSES_COMMAND, l = strlen(TEST_PROCESSES_COMMAND))){
        if (buf[l] != ' ' && buf[l] != 0){
            printErrorMessage(buf, COMMAND_NOT_FOUND_MESSAGE);
            printNewline();
            return 1;
        }
        long read = readDecimalInput(buf + l);
        char * readS = "";
        itoa(read, readS);
        if (readS == NULL){
            printErrorMessage(TEST_PROCESSES_COMMAND, "No argument received");
            printNewline();
            return 1;
        }
        char *argv[] = {readS};
        ProcessesTest(argv);
    } else if (!strcmp(buf, TEST_PRIORITY_COMMAND)){
        PrioTest();
    } else if (!strcmp(buf, PRINT_HEAP_STATUS_COMMAND)) {
        memStatus();
    } else if (!strcmp(buf, PID_COMMAND)) {
        printf("PID: %d\n", getpid());
    } else if (!strncmp(buf, TEST_MM_COMMAND, l = strlen(TEST_MM_COMMAND))){
        if (buf[l] != ' ' && buf[l] != 0){
            printErrorMessage(buf, COMMAND_NOT_FOUND_MESSAGE);
            printNewline();
            return 1;
        }
        long read = readDecimalInput(buf + l);
        char * readS = "";
        itoa(read, readS);
        if (readS == NULL){
            printErrorMessage(TEST_MM_COMMAND, "No argument received");
            printNewline();
            return 1;
        }
        char *argv[] = {readS};
        testMemory(argv);
    } else if (!strncmp(buf, TEST_SYNC_COMMAND, l = strlen(TEST_SYNC_COMMAND))) {
        if (buf[l] != ' ' && buf[l] != 0){
            printError(COMMAND_NOT_FOUND_MESSAGE);
            printNewline();
            return 1;
        }
        long times = readDecimalInput(buf + l);
        if (times == -1)
            return 1;
        char * arg = itoa2(times);
        long number = readDecimalInput(buf + l + 2);
        if (number == -1)
            return 1;
        char * arg1 = itoa2(number);
        char * argv[] = {arg, arg1, NULL}; // MUST BE NULL TERMINATED 
        SyncTest(argv);
    } else {
        printError(COMMAND_NOT_FOUND_MESSAGE);
        printNewline();
    }
    return 1;
}

void printErrorMessage(char * program, char * errorMessage){
    printString(SHELL_NAME, GREEN);
    printf(" : %s : ", program);
    printError(errorMessage);
}

void helpCommand(){
    printf(HELP_MESSAGE);
    printNewline();
}

void printNewline(){
    printString(NEWLINE,WHITE);
}

void testDivideByZeroException(){
    divideZero();
}


void testInvalidOpException(){
    invalidOpcode();
}

void printInforeg(){
    long array[REGISTER_NUM] = {0};
    long * arr = (long *) &array;
    sys_inforeg(arr);
    char * registerNames[] = REGISTER_NAMES;
    for (int i = 0 ; i < REGISTER_NUM; i++){
        printf("%s : ",registerNames[i]);
        printBase(arr[i], 2);
        printf("b\n");
    }
}

int increaseFontSize(){
    return sys_changeFontSize(INCREASE);
}

int decreaseFontSize(){
    return sys_changeFontSize(DECREASE);
}

void testMemory(char * argv[]) {
    int pid = exec("test_mm", argv, &test_mm, 0, 1);
    waitpid(pid);
}

void ProcessesTest(char * argv[]) {
    int pid = exec("test_processes", argv, &test_processes, 0, 1);
    waitpid(pid);
}

void PrioTest() {
    char *argv[] = {NULL};
    int pid = exec("test_priority", argv, &test_prio, 0, 1);
    waitpid(pid);
}

void SyncTest(char * argv[]) {
    int pid = exec("test_sync", argv, &test_sync, 0, 1);
    waitpid(pid);
}

long readDecimalInput(char * buf) {
    int i = 0;
    while (buf[i] != 0 && buf[i] == ' ')
        i++;
    if (buf[i] == 0){
        printErrorMessage(PRINTMEM_COMMAND, "No argument received");
        printNewline();
        return -1;
    }
    long accum = 0;
    for (; buf[i] != 0 ; i++){
        if (buf[i] >= '0' && buf[i] <= '9') {
            accum = 10*accum + buf[i] - '0';
        } else if (buf[i] == ' ' || buf[i] == 0) {
            i++;
        } else {
            printErrorMessage(PRINTMEM_COMMAND, "Argument must be a decimal value");
            printNewline();
            return -1;
        }
    }
    return accum;
}