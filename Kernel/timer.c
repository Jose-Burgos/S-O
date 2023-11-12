// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <time.h>
#include <stdint.h>

static unsigned long ticks = 0;

int getSeconds();
int getMinutes();
int getHours();
int getDay();
int getMonth();
int getYear();

void timer_handler() {
	ticks++;
}

int ticks_elapsed() {
	return ticks;
}

int seconds_elapsed() {
	return ((double) ticks/18);
}

uint64_t getTime() {
	return getHours() | ((uint64_t) getMinutes() << 8) | ((uint64_t) getSeconds() << 16);
}

uint64_t getDate() {
	return getDay() | ((uint64_t)getMonth() << 8) | ((uint64_t)getYear() << 16);
}

void wait(int t){
    int initial = ticks;
    while (ticks - initial < t);
}


long milliseconds_elapsed() {
	return 1000 * ((double) ticks / 18);
}




