#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <limits.h>

#include "../include/stack.h"

void *myMemCpy(void *dest, void *src, size_t n) {
    char *cDest = (char *)dest;
    char *cSrc = (char *)src;
    while (true) {
        if (n >= 8) {
            long long *llDest = (long long *)cDest;
            *llDest = *(long long *)cSrc;
            cDest += 8;
            cSrc += 8;
            n -= 8;
        } else if (n >= 4) {
            int *iDest = (int *)cDest;
            *iDest = *(int *)cSrc;
            cDest += 4;
            cSrc += 4;
            n -= 4;
        } else if (n >= 2) {
            short *sDest = (short *)cDest;
            *sDest = *(short *)cSrc;
            cDest += 2;
            cSrc += 2;
            n -= 2;
        } else if (n >= 1) {
            *cDest = *cSrc;
            ++cDest;
            ++cSrc;
            --n;
        } else {
            break;
        }
    }
    return dest;
}

int StackError(Stack *stack) {
    if (!stack) {
        return STK_NULL;
    }
    if (stack->size > stack->capacity) {
        return STK_CAP_OVERFL;
    }
    return 0;
}

int StackDump_(Stack *stack, int errCode, const char *reason, callInfo info) {
    printf("Dumping stack from %s() in %s at (%d)\n", stack->funcName, stack->file, stack->line);
    printf("Dump called from %s() in %s at (%d)\n", info.funcName, info.file, info.line);
    return 1;
}

int StackCtor_(Stack *stack, size_t el_size, size_t capacity, callInfo info) {
    stack->data = calloc(capacity, el_size);
    if (stack->data == nullptr) {
        printf("There was an error allocating memory for the stack : %s\n", strerror(errno));
        return STACK_NOMMRY;
    }
    stack->size = 0;
    stack->elem_size = el_size;
    stack->capacity = capacity;

#ifdef DEBUG_MODE
    stack->funcName = info.funcName;
    stack->file = info.file;
    stack->line = info.line; 

    StackDump(stack, 0, "meme"); 
    ASSERT_OK(stack);
#endif
    return 1;
}

int StackResize(Stack *stack, size_t size) {
#ifdef DEBUG_MODE
    ASSERT_OK(stack);
#endif

    const size_t newCap = size;
    void *newData = realloc(stack->data, newCap * stack->elem_size);
    if (newData == nullptr) {
        printf("There was an error allocating memory : %s\n", strerror(errno));
        return 0;
    }
    printf("Resizing to %zu\n", newCap);
    stack->data = newData;
    stack->capacity = newCap;

#ifdef DEBUG_MODE
    ASSERT_OK(stack);
#endif
    return 1;
}

void StackDtor(Stack *stack) {
#ifdef DEBUG_MODE
    ASSERT_OK(stack);
#endif

    memset(stack->data, 0xF0, stack->capacity * stack->elem_size);
    stack->size = UINT_MAX;
    free(stack->data);
    stack->data = (int *)13;
}

void StackPush(Stack *stack, void *src, int *err) {
#ifdef DEBUG_MODE
    ASSERT_OK(stack);
    assert(src);
#endif

    if (stack->size == stack->capacity) {
        if (StackResize(stack, stack->capacity * 2) == 0) {
            printf("There was an error growing stack\n");
            if (err) {
                *err = STACK_NOMMRY;
            }
            return;
        }
    }
    myMemCpy((char *)stack->data + stack->size++ * stack->elem_size, src, stack->elem_size);

#ifdef DEBUG_MODE
    ASSERT_OK(stack);
#endif
}

void StackPop(Stack *stack, void *dest, int *err) {
#ifdef DEBUG_MODE
    ASSERT_OK(stack);
    assert(dest);
#endif

    if (stack->size == 0) {
        printf("Can't pop from an empty stack\n");
        if (err) {
            *err = STACK_UNDERFL;
        } 
        return;
    }
    myMemCpy(dest, (char *)stack->data + --stack->size * stack->elem_size, stack->elem_size);
    if (stack->size > 0 && stack->size == stack->capacity / 4) {
        if (StackResize(stack, stack->capacity / 2) == 0) {
            printf("There was an error shrinking stack\n");
            if (err) {
                *err = STACK_NOMMRY;
            }
            return;
        }
    }

#ifdef DEBUG_MODE
    ASSERT_OK(stack);
#endif
}

void StackPrint(Stack *stack) {
    for (size_t i = 0; i < stack->size; i++) {
        printf("%d\n", (int)*((char *)stack->data + i * stack->elem_size));
    }
}
