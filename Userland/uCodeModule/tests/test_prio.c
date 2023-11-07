#include <stdint.h>
//#include "syscall.h" // TODO: Remove this dependency
//#include "test_util.h"
#include <../include/lib.h>

#define MINOR_WAIT 1000000 // TODO: Change this value to prevent a process from flooding the screen
#define WAIT 10000000      // TODO: Change this value to make the wait long enough to see theese processes beeing run at least twice

#define TOTAL_PROCESSES 3
#define LOWEST 0  // TODO: Change as required
#define MEDIUM 3  // TODO: Change as required
#define HIGHEST 6 // TODO: Change as required

int64_t prio[TOTAL_PROCESSES] = {LOWEST, MEDIUM, HIGHEST};

void testing(int i) {
  for (size_t i = 0; i < 100000; i++) {
    shortSleep(5);
    printf("%d\n", i);
  }
}

void test_prio() {
  int64_t pids[TOTAL_PROCESSES];
  uint64_t i;

  for (i = 0; i < TOTAL_PROCESSES; i++) {
    char *argv[] = {i};
    pids[i] = exec("loop_print", argv, &testing, prio[i], 0);
  }

  bussy_wait(WAIT);
  printf("\nCHANGING PRIORITIES...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    my_nice(pids[i], prio[i]);

  bussy_wait(WAIT);
  printf("\nBLOCKING...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    my_block(pids[i]);

  printf("CHANGING PRIORITIES WHILE BLOCKED...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    my_nice(pids[i], MEDIUM);

  printf("UNBLOCKING...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    my_unblock(pids[i]);

  bussy_wait(WAIT);
  printf("\nKILLING...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    my_kill(pids[i]);
}
