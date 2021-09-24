#include <sys/types.h>

typedef int elem_t;

struct Stack {
    elem_t *data;
    size_t size;
    size_t capacity;
};

int StackCtor(Stack *stack, size_t capacity);

void StackDtor(Stack *stack);

int StackPop(Stack *stack, elem_t *x);

int StackPush(Stack *stack, elem_t x);

int StackResize(Stack *stack, size_t size);

void StackPrint(Stack *stack);
