#ifndef STACK_H
#define STACK_H

#include <sys/types.h>

enum {
    STACK_UNDERFL,
    STACK_NOMMRY
};

struct Stack {
    void *data;
    size_t elem_size;
    size_t size;
    size_t capacity;
};

void *myMemCpy(void *dest, void *src, size_t n);

void StackCtor(Stack *stack, size_t el_size, size_t capacity, int *err = nullptr);

void StackDtor(Stack *stack);

void StackPop(Stack *stack, void *dest, int *err = nullptr);

void StackPush(Stack *stack, void *src, int *err = nullptr);

int StackResize(Stack *stack, size_t size);

void StackPrint(Stack *stack);

#endif
