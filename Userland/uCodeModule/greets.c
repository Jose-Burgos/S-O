#include "include/lib.h"
#include <stdbool.h>
#include "include/greets.h"

void greets(void) {
    while (1) {
        printf("Hello i'm %d, have a great day!\n", getpid());
        shortSleep(1500);
    }
}
