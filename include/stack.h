#ifndef STACK_H
#define STACK_H

#include <sys/types.h>

#ifndef __FUNCTION_NAME__
    #ifdef WIN32   //WINDOWS
        #define __FUNCTION_NAME__   __FUNCTION__  
    #else          //*NIX
        #define __FUNCTION_NAME__   __func__ 
    #endif
#endif

#define DEBUG_MODE

//------------------ User type -------------

struct myStruct {
    long long a;
    long long b;
};

static const char *typeName = "myStruct";

typedef myStruct elem_t;

char *formatInstance(const elem_t *instance);

//------------------------------------------

static const int STK_POISON = 0x0F;

enum StkError : int {
    STK_NULL = 1,
    STK_NEG_CAP = 2,
    STK_CAP_OVERFL = 3,
    STK_UNDERFL = 4,
    STK_NOMMRY = 5,
};

struct callInfo {
    const char *funcName;
    const char *file;
    int line;
};

struct Stack {
    void *data;
    size_t elemSize;
    size_t size;
    size_t capacity;

#ifdef DEBUG_MODE
    const char *typeName;

    const char *funcName;
    const char *file;
    int line;
#endif
};

const size_t DEFAULT_STACK_CAPACITY = 10;

void *myMemCpy(void *dest, void *src, size_t n);

int StackError(Stack *stack);

int StackDump_(Stack *stack, const char *reason, callInfo info, const char *stkName);

int StackCtor_(Stack *stack, size_t el_size, size_t capacity, callInfo info);

void StackDtor(Stack *stack);

void StackPop(Stack *stack, void *dest, int *err = nullptr);

void StackPush(Stack *stack, void *src, int *err = nullptr);

int StackResize(Stack *stack, size_t size);

#define ASSERT_OK(STACK)                        \
do {                                            \
    int ret = StackError(STACK);                \
    if (ret != 0) {                             \
        StackDump(STACK, "ASSERT_OK failed");   \
        assert(!"Invariant failure");           \
    }                                           \
} while (0)

#define StackCtor(stack, el_size, capacity)    \
do {                                           \
    callInfo inf = {};                         \
    inf.funcName = __FUNCTION_NAME__;          \
    inf.file = __FILE__;                       \
    inf.line = __LINE__;                       \
    StackCtor_(stack, el_size, capacity, inf); \
} while (0)

#define StackDump(stack, reason)      \
do {                                           \
    callInfo inf = {};                         \
    inf.funcName = __FUNCTION_NAME__;          \
    inf.file = __FILE__;                       \
    inf.line = __LINE__;                       \
    StackDump_(stack, reason, inf, #stack);   \
} while (0)

#endif
