#include "../include/test.h"

int torture() {
    TortureChamber chamb = {};

    StackCtor(&chamb.victim, sizeof(elem_t), 10);

    for (size_t i = 0; i < 5; ++i) {
        chamb.execLeft[i] = 'j';
    }

    return 1;
}
