#ifndef STACK_H
#define STACK_H

#include <sys/types.h>
#include <limits.h>

#ifndef __FUNCTION_NAME__
    #ifdef WIN32   //WINDOWS
        #define __FUNCTION_NAME__   __FUNCTION__  
    #else          //*NIX
        #define __FUNCTION_NAME__   __func__ 
    #endif
#endif

#define DEBUG_MODE

//------------------ User type -------------

//! User defined type for use in the stack
struct myStruct {
    long long a;
    long long b;
};

//! String constant for name of the user type for debug output
static const char *typeName = "myStruct";

//! Typedef for universal interface
typedef myStruct elem_t;

//------------------------------------------------------------ 
//! User defined function that formats their data type to a string for debug output
//!
//! @param[in]  instance    Pointer to user type instance to format
//!
//! @return String version of the user type instance on success, nullptr on failure
//------------------------------------------------------------ 
char *formatInstance(const elem_t *instance);

//------------------------------------------------------------ 

//! Constant used by stack destructor to designate bytes it has already freed
static const int STK_DATA_POISON = 0x0F;
//! Constant for stack size used to prevent double destructor calls
static const size_t STK_SIZE_POISON = UINT_MAX; 

//! Encapsulates errors that can occur with the stack
enum StkError : int {
    STK_NULL = 1, /**< Stack pointer is null */
    STK_INV_SIZE = 2, /**< Stack size has a value of STK_SIZE_POISON */ 
    STK_CAP_OVERFL = 3, /**< Stack size is more than stack capacity */
    STK_UNDERFL = 4, /**< Trying to pop from an empty stack */
    STK_NOMMRY = 5, /**< Memory allocation error */
};

//! Encapsulates info about function call for debug
struct callInfo {
    const char *funcName; /**< Name of the calling function */
    const char *file; /**< File where the function call happened */
    int line; /**< Line of the function call in the file */
};

//! Declares a stack data structure as a struct
struct Stack {
    void *data; /**< Pointer to the stack data array */
    size_t elemSize; /**< Size of the stored type in bytes */
    size_t size; /**< Current number of elements in the stack */
    size_t capacity; /**< Current full capacity of the array */ 

#ifdef DEBUG_MODE
    const char *typeName; /**< String name of the data type stored in the stack for debug output */

    const char *ctorCallFuncName; /**< Function that called the stack constructor */
    const char *ctorCallFile; /**< File of the constructor call */
    int ctorCallLine; /**< Line of the constructor call */
#endif
};

//! Default stack capacity
const size_t DEFAULT_STACK_CAPACITY = 10;

int StackError(Stack *stack);

int writeErrCode(int err);

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
