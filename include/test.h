#ifndef TEST_H
#define TEST_H

#include "stack.h"

struct TortureChamber {
    int execLeft[1];
    Stack victim;
    int execRight[1];
};

void tortureLeft();

void tortureRight();

int testStack();

#endif
