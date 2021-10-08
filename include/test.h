#ifndef TEST_H
#define TEST_H

#include "stack.h"

struct TortureChamber {
    char execLeft[1];
    Stack victim;
    char execRight[1];
};

int torture();

#endif
