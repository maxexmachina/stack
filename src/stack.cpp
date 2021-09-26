#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <limits.h>

#include "../include/stack.h"

int StackCtor(Stack *stack, size_t capacity) {
    assert(stack);
    stack->data = (elem_t *)calloc(capacity, sizeof(elem_t));
    if (stack->data == nullptr) {
        printf("There was an error constructing stack : %s\n", strerror(errno));
        return 0;
    }
    stack->size = 0;
    stack->capacity = capacity;
    return 1;
}

int StackResize(Stack *stack, size_t size) {
    assert(stack);
    const size_t newCap = size;
    int *newData = (elem_t *)realloc(stack->data, newCap * sizeof(elem_t));
    if (newData == nullptr) {
        printf("There was an error allocating memory : %s\n", strerror(errno));
        return 0;
    }
    printf("Resizing to %zu\n", newCap);
    stack->data = newData;
    stack->capacity = newCap;
    return 1;
}

void StackDtor(Stack *stack) {
    assert(stack);
    memset(stack->data, 0xF0, stack->capacity);
    stack->size = UINT_MAX;
    free(stack->data);
    stack->data = (int *)13;
}

int StackPush(Stack *stack, elem_t x) {
    assert(stack);
    if (stack->size == stack->capacity) {
        if (StackResize(stack, stack->capacity * 2) == 0) {
            printf("There was an error growing stack\n");
            return 0;
        }
    }
    //printf("pushing %d\n", x);
    stack->data[stack->size++] = x;
    return 1;
}

int StackPop(Stack *stack, elem_t *x) {
    assert(stack);
    if (stack->size == 0) {
        printf("Can't pop from an empty stack\n");
        return 0;
    }
    *x = stack->data[--stack->size];
    if (stack->size > 0 && stack->size == stack->capacity / 4) {
        if (StackResize(stack, stack->capacity / 2) == 0) {
            printf("There was an error shrinking stack\n");
            return 0;
        }
    }
    return 1;
}

void StackPrint(Stack *stack) {
    for (size_t i = 0; i < stack->size; i++) {
        printf("%d\n", stack->data[i]);
    }
}
