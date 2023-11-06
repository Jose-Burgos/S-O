/* uCodeModule.c */
#include <color.h>
#include <timer.h>
#include <lib.h>

void shell(void);

char *v = (char *)0xB8000 + 79 * 2;

int main() {
    clear();

    //yield();
    //char *argv[] = {"shell"};
    //exec("shell", argv, &shell, 0, 1);
    shell();
    return 0;
}
