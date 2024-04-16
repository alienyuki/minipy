#include "vm.h"
#include "object.h"
#include "frame_object.h"

#include <stdlib.h>
#include <stdio.h>

int pvm_run(pvm* vm, CodeObject* code) {
    FrameObject* frame = (FrameObject*) init_frame((Object*) code);
    int err = 0;
    vm->frame = frame;
    vm->pc = ((CodeObject*) frame->code)->bytecodes;

    while (!err) {
        switch (*vm->pc) {
        default: {
            printf("opcode: 0x%x is not implement yet\n", *vm->pc);
            err = 1;
        }

        }
    }
    DECREF(frame);

    return err;
}

pvm* vm_init() {
    return malloc(sizeof(pvm));
}

void vm_destroy(pvm* vm) {
    free(vm);
}
