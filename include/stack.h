#include <sys/types.h>

struct Stack {
    int *data;
    size_t size;
    size_t capacity;
};

int StackCtor(Stack *stack, size_t capacity);

void StackDtor(Stack *stack);

int StackPop(Stack *stack, int *x);

int StackPush(Stack *stack, int x);

int StackResize(Stack *stack, size_t size);

void StackPrint(Stack *stack);
