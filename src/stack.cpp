#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <limits.h>

#include "../include/stack.h"
#include "../include/log.h"

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

char *formatInstance(const elem_t *instance) {
    if (!instance) {
        return nullptr;
    }
    char *formatted = (char *)calloc(64, sizeof(*formatted));
    if (formatted == nullptr) {
        return nullptr;
    }
    if (sprintf(formatted, "a : %lld, b : %lld", instance->a, instance->b) < 0) {
        free(formatted);
        return nullptr;
    }
    return formatted;
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

int writeErrCode(int err) {
    switch (err) {
        case STK_NULL:
            writeToLog("STK_NULL ");
            break;
        case STK_NEG_CAP:
            writeToLog("STK_NEG_CAP ");
            break;
        case STK_CAP_OVERFL:
            writeToLog("STK_CAP_OVERFL ");
            break;
        default:
            writeToLog("UNDEFINED ");
    }
    return 1;
}

#ifdef DEBUG_MODE
int StackDump_(Stack *stack, const char *reason, callInfo info, const char *stkName) {
    int errCode = StackError(stack);
    writeToLog("Stack<%s>[%p] called from %s() at %s (%d) ", 
                typeName, (void *)stack, info.funcName, info.file, info.line);

    if (errCode == 0) {
        writeToLog("ok ");
    } else {
        writeErrCode(errCode);
    }

    writeToLog("%s at %s() at %s (%d)\n"
               "{\n"
               "Dump reason : %s\n",
               stkName, stack->funcName, stack->file, stack->line, reason);

    if (!stack) {
        writeToLog("stack : nullptr\n");
    } else {
        writeToLog("size = %zu\ncapacity = %zu\nelement size = %zu\ndata[%p]:\n"
                   "{\n", stack->size, stack->capacity, stack->elemSize, (void *)stack->data);

        if (!stack->data) {
            writeToLog("stack->data : nullptr\n");
        } else {
            for (size_t i = 0; i < stack->capacity; ++i) {
                if (i < stack->size) {
                    writeToLog("*[%zu] = ", i);
                    char *format = formatInstance((elem_t *)((char *)stack->data + i * stack->elemSize));
                    if (format != nullptr) {
                        writeToLog("%s\n", format); 
                        free(format);
                    }
                } else {
                    char curChar = *(char *)((char *)stack->data + i * stack->elemSize);
                    writeToLog("[%zu] = %x ", i, curChar);
                    if (curChar == STK_POISON) {
                        writeToLog("(POISON)");
                    }
                    writeToLog("\n");
                }
            }
        }
        writeToLog("}\n"
                   "}\n");
    }

    if (errCode != 0) {
        closeLog();
    }
    return 1;
}
#endif

int StackCtor_(Stack *stack, size_t el_size, size_t capacity, callInfo info) {
    if (capacity != 0) {
        stack->data = calloc(capacity, el_size);
        if (stack->data == nullptr) {
            printf("There was an error allocating memory for the stack : %s\n", strerror(errno));
            return STK_NOMMRY;
        }
    }
    stack->size = 0;
    stack->elemSize = el_size;
    stack->capacity = capacity;

#ifdef DEBUG_MODE
    stack->funcName = info.funcName;
    stack->file = info.file;
    stack->line = info.line; 

    ASSERT_OK(stack);
#endif
    return 1;
}

int StackResize(Stack *stack, size_t size) {
#ifdef DEBUG_MODE
    ASSERT_OK(stack);
#endif

    const size_t newCap = size;
    void *newData = realloc(stack->data, newCap * stack->elemSize);
    if (newData == nullptr) {
        printf("There was an error allocating memory : %s\n", strerror(errno));
        return 0;
    }
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

    memset(stack->data, STK_POISON, stack->capacity * stack->elemSize);
#ifdef DEBUG_MODE
    StackDump(stack, "dtor");
#endif
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
        size_t newCap = 0;
        if (stack->capacity == 0) {
            newCap = DEFAULT_STACK_CAPACITY;
        } else {
            newCap = stack->capacity * 2;
        }
        if (StackResize(stack, newCap) == 0) {
            printf("There was an error growing stack\n");
            if (err) {
                *err = STK_NOMMRY;
            }
            return;
        }
    }
    myMemCpy((char *)stack->data + stack->size++ * stack->elemSize, src, stack->elemSize);

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
            *err = STK_UNDERFL;
        } 
        return;
    }
    myMemCpy(dest, (char *)stack->data + --stack->size * stack->elemSize, stack->elemSize);
    if (stack->size > 0 && stack->size == stack->capacity / 4) {
        if (StackResize(stack, stack->capacity / 2) == 0) {
            printf("There was an error shrinking stack\n");
            if (err) {
                *err = STK_NOMMRY;
            }
            return;
        }
    }

#ifdef DEBUG_MODE
    ASSERT_OK(stack);
#endif
}
