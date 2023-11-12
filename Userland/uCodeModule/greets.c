// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "include/lib.h"
#include <stdbool.h>
#include "include/greets.h"

void greets(void) {
    while (1) {
        printf("Hello i'm %l, have a great day!\n", getpid());
        shortSleep(1500);
    }
}
