#ifndef _TST_H_
#define _TST_H_

#include "test_util.h"
#include "../../include/lib.h"

uint64_t test_mm(uint64_t argc, char *argv[]);
uint64_t test_processes(uint64_t argc, char *argv[]);
void test_prio();
uint64_t test_sync(uint64_t argc, char *argv[]);
#endif