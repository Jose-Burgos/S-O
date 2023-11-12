// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <timer.h>
#include <lib.h>
#include <syscalls.h>

void getDateFormat(char *buffer);
void getTimeFormat(char *buffer);

/**
 * @brief Get the Current Date
 *
 * @return char*
 */
void getDateAndTime(char *buff) {
    char *p = buff;
    getDateFormat(p);
    p[8] = ' ';
    getTimeFormat(&p[9]);
}

void getTimeFormat(char *buff) {
    char *p = buff;
    long time = sys_time();
    int hours = ((time)&0xFF), minutes = ((time >> 8) & 0xFF), seconds = ((time >> 16) & 0xFF);
    if (hours > 3)
        hours -= 3;
    if (hours == 0)
        hours = 21;
    if (hours == 1)
        hours = 22;
    if (hours == 2)
        hours = 23;
    itoa(hours, p);
    p[2] = ':';
    itoa(minutes, &p[3]);
    p[5] = ':';
    itoa(seconds, &p[6]);
    p[8] = 0;
}

void getDateFormat(char *buff) {
    char* p = buff;
	long date = sys_date();
	itoa((date & 0xFF), p);
	p[2] = '/';
	itoa((date >> 8) & 0xFF, &p[3]);
	p[5] = '/';
	itoa((date >> 16) & 0xFF, &p[6]);
    p[8] = 0;
}
