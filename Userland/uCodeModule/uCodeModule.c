/* uCodeModule.c */
#include <syscalls.h>
#include <color.h>
#include <timer.h>
#include <lib.h>

void shell(void);

char *v = (char *)0xB8000 + 79 * 2;

int main() {
    sys_clearScreen();
    //yield();
    char *argv[] = {"shell"};
    exec("shell", argv, &shell, 0, 1);
    //shell();
    return 0;
}
