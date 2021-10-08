#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "../include/stack.h"
#include "../include/log.h"
#include "../include/test.h"

int main() {
    testStack();

    if (closeLog() != 1) {
        printf("There was an error closing log file : %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
