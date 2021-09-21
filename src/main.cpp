#include <stdio.h>

#include "../include/stack.h"

int main() {
    Stack stack = {};

    StackPush(&stack, 10);
    StackPush(&stack, 1);
    StackPush(&stack, 13);
    int val = 0;
    StackPop(&stack, &val);
    printf("val : %d\n", val);
    StackPush(&stack, 14);

    StackPrint(&stack);

    StackDtor(&stack);
}
