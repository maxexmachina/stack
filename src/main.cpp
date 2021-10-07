#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "../include/stack.h"
#include "../include/log.h"

int main() {
    Stack stack = {};
    elem_t v = {.a = 12341234, .b = 12341235};
    StackCtor(&stack, sizeof(elem_t), 2);
    for (size_t i = 0; i < 100; ++i) {
        StackPush(&stack, &v); 
    }

#if DEBUG_MODE > 0
    StackDump(&stack, "Diagnostic");
#endif

    for (size_t i = 0; i < 100; ++i) {
        StackPop(&stack, &v);
    }
    StackDtor(&stack);

    if (closeLog() != 1) {
        printf("There was an error closing log file : %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
