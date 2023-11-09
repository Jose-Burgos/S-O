/* uCodeModule.c */
#include <color.h>
#include <timer.h>
#include "include/lib.h"

void shell(int argc, char ** argv);

char *v = (char *)0xB8000 + 79 * 2;

int main() {
    clear();
    char *argv[] = {NULL};

    int pid = exec("shell", argv, &shell, 0, 1);

    if (pid == -1) {
        printf("uCodeModule %s\n", "Error creating shell process");
        return -1;
    }

    while (1) { // INIT process
        shortSleep(1);
    }
    
    return 0;
}
