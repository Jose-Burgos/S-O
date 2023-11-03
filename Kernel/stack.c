#include "stack.h"
#include <scheduler.h>

stackFrame createStack(void *entryPoint, void *stackBase, uint64_t argc, char **argv)
{
    stackFrame sf;
    sf.r15 = 0x0;
    sf.r14 = 0x0;
    sf.r13 = 0x0;
    sf.r12 = 0x0;
    sf.r11 = 0x0;
    sf.r10 = 0x0;
    sf.r9 = 0x0;
    sf.r8 = 0x0;
    sf.rsi = (uint64_t)argv;
    sf.rdi = argc;
    sf.rbp = 0x0;
    sf.rdx = 0x0;
    sf.rcx = 0x0;
    sf.rbx = 0x0;
    sf.rax = 0x0;
    sf.ip = entryPoint;
    sf.cs = 0x8;
    sf.eflags = 0x202;
    sf.sp = (uint64_t)stackBase;
    sf.ss = 0x0;
    sf.returnAddress = (uint64_t)&killCurrentProcess;
    return sf;
}