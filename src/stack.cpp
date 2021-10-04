#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <limits.h>

#include "../include/mymemcpy.h"
#include "../include/stack.h"
#include "../include/log.h"

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

elem_t getPoisonedInstance() {
    return {.a = LLONG_MAX, .b = LLONG_MIN};
}

bool isEqualBytes(const void *elem1, const void *elem2, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        if (*((const char *)elem1 + i) != *((const char *)elem2 + i)) {
            return false;
        }
    }
    return true;
}

#if DEBUG_MODE > 1
canary_t *getDataCanaryRight(Stack *stack) {
    return (canary_t *)getIndexAdress(stack->data, stack->capacity, stack->elemSize);
}

canary_t *getDataCanaryLeft(Stack *stack) {
    return (canary_t *)((char *)stack->data - sizeof(canary_t));
}
#endif

#if DEBUG_MODE > 0 
int StackError(Stack *stack) {
    if (!stack) {
        return STK_NULL;
    }
    if (stack->size > stack->capacity) {
        return STK_CAP_OVERFL;
    }
    if (stack->size == STK_SIZE_POISON) {
        return STK_INV_SIZE;
    }
#if DEBUG_MODE > 1
    if (stack->canaryLeft != canaryVal) {
        return STK_CAN_LFT;
    }
    if (stack->canaryRight != canaryVal) {
        return STK_CAN_RGT;
    }
    if (*getDataCanaryLeft(stack) != canaryVal) {
        return STK_DATA_CAN_LFT;
    }
    if (*getDataCanaryRight(stack) != canaryVal) {
        return STK_DATA_CAN_RGT;
    }
#endif
    return 0;
}

int writeErrCode(int err) {
    int res = 0;
    switch (err) {
        case STK_NULL:
            res = writeToLog("STK_NULL ");
            break;
        case STK_INV_SIZE:
            res = writeToLog("STK_INV_SIZE ");
            break;
        case STK_CAP_OVERFL:
            res = writeToLog("STK_CAP_OVERFL ");
            break;
#if DEBUG_MODE > 1 
        case STK_CAN_LFT:
            res = writeToLog("STK_CAN_LFT ");
            break;
        case STK_CAN_RGT:
            res = writeToLog("STK_CAN_RGT ");
            break;
        case STK_DATA_CAN_LFT:
            res = writeToLog("STK_DATA_CAN_LFT ");
            break;
        case STK_DATA_CAN_RGT:
            res = writeToLog("STK_DATA_CAN_RGT ");
            break;
#endif
        default:
            res = writeToLog("UNDEFINED ");
    }
    return res >= 0;
}
#endif

void *getIndexAdress(void *start, size_t index, size_t size) {
    return (char *)start + index * size; 
}

#if DEBUG_MODE > 0
int StackDump_(Stack *stack, const char *reason, callInfo info, const char *stkName) {
    int dumpResult = 1;

    int errCode = StackError(stack);
    dumpResult *= writeToLog("Stack<%s>[%p] called from %s() at %s (%d) ", 
                             typeName, (void *)stack, info.funcName, info.file, info.line);

    if (errCode == 0) {
        dumpResult *= writeToLog("ok ");
    } else {
        dumpResult *= writeErrCode(errCode);
    }

    dumpResult *= writeToLog("%s at %s() at %s (%d)\n"
                             "{\n"
                             "Dump reason : %s\n",
                             stkName, stack->ctorCallFuncName, stack->ctorCallFile, stack->ctorCallLine, reason);

    if (!stack) {
        dumpResult *= writeToLog("stack : nullptr\n");
    } else {
#if DEBUG_MODE > 1
        dumpResult *= writeToLog("Left canary: %llx\nRight canary: %llx\n"
                                 "Left data canary: %llx\nRight data canary: %llx\n",
                                 stack->canaryLeft, stack->canaryRight,
                                 *getDataCanaryLeft(stack), *getDataCanaryRight(stack));
#endif
        dumpResult *= writeToLog("size = %zu\ncapacity = %zu\nelement size = %zu\ndata[%p]:\n"
                                 "{\n", stack->size, stack->capacity, stack->elemSize, (void *)stack->data);

        if (!stack->data) {
            dumpResult *= writeToLog("stack->data : nullptr\n");
        } else {
            for (size_t i = 0; i < stack->capacity; ++i) {
                if (i < stack->size) {
                    dumpResult *= writeToLog("*");
                }
                dumpResult *= writeToLog("[%zu] = ", i);

                elem_t *cur = (elem_t *)getIndexAdress(stack->data, i, stack->elemSize);
                char *format = formatInstance(cur);
                if (format != nullptr) {
                    dumpResult *= writeToLog("%s", format); 
                    free(format);
                }
                elem_t poison = getPoisonedInstance();
                if (isEqualBytes(cur, &poison, stack->elemSize)) {
                    dumpResult *= writeToLog(" (POISON)");
                }
                dumpResult *= writeToLog("\n");
            }
        }
        dumpResult *= writeToLog("}\n"
                                 "}\n");
    }
    if (dumpResult == 0) {
        printf("Dumping error\n");
        return 0;
    } 

    if (errCode != 0) {
        closeLog();
    }
    return 1;
}
#endif

int StackCtor_(Stack *stack, size_t elemSize, size_t capacity, callInfo info) {
#if DEBUG_MODE > 0
    assert(stack);
#endif

    if (capacity != 0) {
#if DEBUG_MODE > 1
        void *data = malloc(elemSize * capacity + 2 * sizeof(canary_t)); 
        if (data == nullptr) {
            printf("There was an error allocating memory for the stack : %s\n", strerror(errno));
            return STK_NOMMRY;
        }
        *(canary_t *)data = canaryVal;
        stack->data = (char *)data + sizeof(canary_t);
        canary_t *canRight = (canary_t *)getIndexAdress(stack->data, capacity, elemSize);
        *canRight = canaryVal;
#else 
        stack->data = calloc(capacity, elemSize);
        if (stack->data == nullptr) {
            printf("There was an error allocating memory for the stack : %s\n", strerror(errno));
            return STK_NOMMRY;
        }
#endif

    }
    stack->size = 0;
    stack->elemSize = elemSize;
    stack->capacity = capacity;

#if DEBUG_MODE > 0
    stack->ctorCallFuncName = info.funcName;
    stack->ctorCallFile = info.file;
    stack->ctorCallLine = info.line; 

#if DEBUG_MODE > 1
    stack->canaryLeft = canaryVal; 
    stack->canaryRight = canaryVal; 
#endif

    ASSERT_OK(stack);
#endif

    return 1;
}

int StackResize(Stack *stack, size_t size) {
#if DEBUG_MODE > 0
    ASSERT_OK(stack);
#endif

    const size_t newCap = size;
#if DEBUG_MODE > 1
    void *newData = realloc((char *)stack->data - sizeof(canary_t), newCap * stack->elemSize + 2 * sizeof(canary_t));
#else
    void *newData = realloc(stack->data, newCap * stack->elemSize);
#endif

    if (newData == nullptr) {
        printf("There was an error allocating memory : %s\n", strerror(errno));
        return 0;
    }

#if DEBUG_MODE > 1
    *(canary_t *)newData = canaryVal;
    stack->data = (char *)newData + sizeof(canary_t);
    canary_t *canRight = (canary_t *)getIndexAdress(stack->data, newCap, stack->elemSize); 
    *canRight = canaryVal;
#else
    stack->data = newData;
#endif
    stack->capacity = newCap;

#if DEBUG_MODE > 0
    ASSERT_OK(stack);
#endif
    return 1;
}

void StackDtor(Stack *stack) {
#if DEBUG_MODE > 0
    ASSERT_OK(stack);

    elem_t poison = getPoisonedInstance();
    for (size_t i = 0; i < stack->capacity; i++) {
        myMemCpy(getIndexAdress(stack->data, i, stack->elemSize),
                 &poison, stack->elemSize);
    }
    StackDump(stack, "Dtor");
    stack->size = STK_SIZE_POISON;
    stack->capacity = STK_SIZE_POISON;
#endif

#if DEBUG_MODE > 1
    free((char *)stack->data - sizeof(canary_t));
#else
    free(stack->data);
#endif

#if DEBUG_MODE > 0
    stack->data = (int *)13;
#endif
}

void StackPush(Stack *stack, void *src, int *err) {
#if DEBUG_MODE > 0
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
    myMemCpy(getIndexAdress(stack->data, stack->size++, stack->elemSize),
             src, stack->elemSize);

#if DEBUG_MODE > 0
    ASSERT_OK(stack);
#endif
}

void StackPop(Stack *stack, void *dest, int *err) {
#if DEBUG_MODE > 0
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

    myMemCpy(dest, getIndexAdress(stack->data, --stack->size, stack->elemSize),
             stack->elemSize);
    if (stack->size > 0 && stack->size == stack->capacity / 4) {
        if (StackResize(stack, stack->capacity / 2) == 0) {
            printf("There was an error shrinking stack\n");
            if (err) {
                *err = STK_NOMMRY;
            }
            return;
        }
    }
#if DEBUG_MODE > 0
    ASSERT_OK(stack);
#endif
}

void StackTop(Stack *stack, void *dest, int *err) {
#if DEBUG_MODE > 0
    ASSERT_OK(stack);
#endif

    if (stack->size == 0) {
        printf("No elements on the stack");
        if (err) {
            *err = STK_UNDERFL;
        } 
        return;
    }
    myMemCpy(dest, getIndexAdress(stack->data, stack->size - 1, stack->elemSize),
             stack->elemSize);

#if DEBUG_MODE > 0 
    ASSERT_OK(stack);
#endif
}
