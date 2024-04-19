#include "vm.h"
#include "object.h"
#include "frame_object.h"
#include "tuple_object.h"
#include "opcode.h"

#include <stdlib.h>
#include <stdio.h>

int pvm_run(pvm* vm, CodeObject* code) {
    FrameObject* frame = init_frame(code);
    int err = 0;
    vm->frame = frame;
    vm->pc = frame->code->bytecodes;
    int nlocalsplus = code->localsplusnames->size;
    vm->sp = frame->localsplus + nlocalsplus;
    printf("frame stack header: %d\n", nlocalsplus);

    while (!err) {
        switch (*vm->pc) {
        case RESUME: {
            vm->pc += 2;
            break;
        }

        case LOAD_CONST: {
            CodeObject* c = frame->code;
            Object* v = tuple_get(c->consts, *(vm->pc + 1));
            object_print(1, v);
            INCREF(v);
            vm->sp += 1;
            vm->sp[-1] = v;
            vm->pc += 2;
            break;
        }

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
