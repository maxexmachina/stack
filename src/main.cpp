#include <stdio.h>

#include "../include/stack.h"

int main() {
    Stack stack = {};
    StackCtor(&stack, 2);

    StackPush(&stack, 10);
    StackPush(&stack, 1);
    StackPush(&stack, 13);
    elem_t val = 0;
    StackPop(&stack, &val);
    printf("val : %d\n", val);
    StackPush(&stack, 14);
    StackPop(&stack, &val);
    StackPop(&stack, &val);
    for (size_t i = 0; i < 10000000; ++i) {
        StackPush(&stack, i);
    }
    for (size_t i = 0; i < 9000000; ++i) {
        StackPop(&stack, &val);
    }

    //StackPrint(&stack);

    StackDtor(&stack);
}
