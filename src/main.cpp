#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "../include/stack.h"
#include "../include/log.h"

struct meme {
    long long a;
    long long b;
};

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

    StackPrint(&stack);

    StackDtor(&stack);

    meme v = {.a = 12341234, .b = 12341235};
    StackCtor(&stack, sizeof(meme), 2);
    for (size_t i = 0; i < 1000; ++i) {
        StackPush(&stack, &v); 
    }

    for (size_t i = 0; i < 900; ++i) {
        StackPop(&stack, &v);
    }
    StackDtor(&stack);

    if (closeLog() != 1) {
        printf("There was an error closing log file : %s\n", strerror(errno));
    }
}
