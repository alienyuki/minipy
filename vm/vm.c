#include "vm.h"
#include "object.h"
#include "frame_object.h"
#include "tuple_object.h"
#include "opcode.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static int pvm_run_frame(pvm* vm) {
    if (vm == NULL) {
        return 0;
    }

    FrameObject* frame = vm->frame;
    int err = 0;
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

        case STORE_NAME: {
            uint8_t arg = *(vm->pc + 1);
            Object* name = tuple_get(frame->code->names, arg);
            Object* stack_top = vm->sp[-1];
            dict_set(frame->locals, name, stack_top);
            DECREF(stack_top);
            vm->sp -= 1;
            vm->pc += 2;

            break;
        }

        case RETURN_CONST: {
            object_print(1, (Object*) frame->locals);
            Object* retval = tuple_get(frame->code->consts, *(vm->pc + 1));
            if (frame->prev_frame != NULL) {
                printf("Not implement yet! %s: %d", __FILE__, __LINE__);
                __builtin_unreachable();
            }
            // set vm->frame to prev_frame
            // destroy current frame
            // push retval to prev_frame's stack

            vm->frame = vm->frame->prev_frame;
            DECREF(frame);

            object_print(1, retval);
            vm->pc += 2;
            goto done;
        }

        default: {
            printf("opcode: 0x%x is not implement yet\n", *vm->pc);
            err = 1;
            break;
        }

        }
    }

done:
    return err;
}


int pvm_run(pvm* vm, CodeObject* code) {
    int err = 0;
    FrameObject* frame = init_frame(code);
    vm->frame = frame;
    frame->prev_frame = NULL;
    frame->next_frame = NULL;
    vm->pc = frame->code->bytecodes;
    int nlocalsplus = code->localsplusnames->size;
    vm->sp = frame->localsplus + nlocalsplus;
    vm->globals = (DictObject*) dict_new();
    frame->locals = vm->globals;
    INCREF(frame->locals);

    pvm_run_frame(vm);

    return err;
}

pvm* vm_init() {
    pvm_run_frame(NULL);
    pvm* vm = malloc(sizeof(pvm));
    memset(vm, 0, sizeof(pvm));
    return vm;
}

void vm_destroy(pvm* vm) {
    if (vm->globals != NULL) {
        DECREF(vm->globals);
    }
    free(vm);
}
