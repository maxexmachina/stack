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

static const char *typeName = "struct meme";

typedef myStruct elem_t;

char *formatInstance(elem_t instance);

//------------------------------------------

enum InvError : int {
    STK_NULL = 1,
    STK_NEG_CAP = 2,
    STK_CAP_OVERFL = 3,
};

enum StkError : int {
    STACK_UNDERFL = 1,
    STACK_NOMMRY = 2,
};

struct callInfo {
    const char *funcName;
    const char *file;
    int line;
};

const size_t DEFAULT_STACK_CAPACITY = 10;

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

void *myMemCpy(void *dest, void *src, size_t n);

int StackError(Stack *stack);

int StackDump_(Stack *stack, int errCode, const char *reason, callInfo info);

int StackCtor_(Stack *stack, size_t el_size, size_t capacity, callInfo info);

void StackDtor(Stack *stack);

void StackPop(Stack *stack, void *dest, int *err = nullptr);

void StackPush(Stack *stack, void *src, int *err = nullptr);

int StackResize(Stack *stack, size_t size);

void StackPrint(Stack *stack);

#define ASSERT_OK(STACK)            \
do {                                \
    int ret = StackError(STACK);    \
    if (ret != 0)                   \
        assert(!"bad" #STACK);      \
} while (0)

#define StackCtor(stack, el_size, capacity)    \
do {                                           \
    callInfo inf = {};                         \
    inf.funcName = __FUNCTION_NAME__;          \
    inf.file = __FILE__;                       \
    inf.line = __LINE__;                       \
    StackCtor_(stack, el_size, capacity, inf); \
} while (0)

#define StackDump(stack, errCode, reason)      \
do {                                           \
    callInfo inf = {};                         \
    inf.funcName = __FUNCTION_NAME__;          \
    inf.file = __FILE__;                       \
    inf.line = __LINE__;                       \
    StackDump_(stack, errCode, reason, inf);   \
} while (0)

#endif
