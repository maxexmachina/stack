#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "../include/stack.h"
#include "../include/log.h"

int main() {
    Stack stack = {};
    int a = 1;
    int b = 10;
    int c = 13;
    int d = 14;

    StackCtor(&stack, sizeof(a), 2);

    StackPush(&stack, &a);
    StackPush(&stack, &b);
    StackPush(&stack, &c);
    int val = 0;
    StackPop(&stack, &val);
    printf("val : %d\n", val);
    StackPush(&stack, &d);
    StackPop(&stack, &val);
    printf("val : %d\n", val);
    StackPop(&stack, &val);
    printf("val : %d\n", val);

    StackDtor(&stack);

    stack = {};
    elem_t v = {.a = 12341234, .b = 12341235};
    StackCtor(&stack, sizeof(elem_t), 2);
    for (size_t i = 0; i < 100; ++i) {
        StackPush(&stack, &v); 
    }
    StackDump(&stack, "Diagnostic");

    for (size_t i = 0; i < 150; ++i) {
        StackPop(&stack, &v);
    }
    StackDtor(&stack);

    if (closeLog() != 1) {
        printf("There was an error closing log file : %s\n", strerror(errno));
    }
}
