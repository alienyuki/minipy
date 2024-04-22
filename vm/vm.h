#ifndef VM_H
#define VM_H

#include "object.h"
#include "code_object.h"
#include "frame_object.h"
#include "dict_object.h"

typedef struct {
    FrameObject* frame;
    DictObject* globals;
    uint8_t* pc;
    Object** sp;
} pvm;

int pvm_run(pvm* vm, CodeObject* code);
pvm* vm_init();
void vm_destroy(pvm* vm);

#endif
