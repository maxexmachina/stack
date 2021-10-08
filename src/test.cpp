#include "../include/test.h"

void tortureLeft() {
    TortureChamber chamb = {};

    StackCtor(&chamb.victim, sizeof(elem_t), 10);

    elem_t elem = {.a = 123, .b = 124};
    StackPush(&chamb.victim, &elem);
    for (size_t i = 0; i < 5; ++i) {
        chamb.execLeft[i] = 0;
    }
    StackPop(&chamb.victim, &elem);


    StackDtor(&chamb.victim);

}

void tortureRight() {
    TortureChamber chamb = {};

    StackCtor(&chamb.victim, sizeof(elem_t), 10);

    elem_t elem = {.a = 123, .b = 124};
    StackPush(&chamb.victim, &elem);
    for (int i = 0; i > -5; --i) {
        chamb.execRight[i] = 0;
    }
    StackPop(&chamb.victim, &elem);

    StackDtor(&chamb.victim);

}

int testStack() {
    Stack stack = {};

    elem_t v = {.a = 12341234, .b = 12341235};
    StackCtor(&stack, sizeof(elem_t), 2);
    for (size_t i = 0; i < 10; ++i) {
        StackPush(&stack, &v); 
    }

#if DEBUG_MODE > 0
    StackDump(&stack, "After 10 pushes");
#endif

    for (size_t i = 0; i < 5; ++i) {
        StackPop(&stack, &v);
    }

#if DEBUG_MODE > 0
    StackDump(&stack, "After 5 pops");
#endif
    StackDtor(&stack);

    //tortureLeft();

    tortureRight();

    return 1;
}
