#ifndef VM_H
#define VM_H

#include "object.h"
#include "code_object.h"
#include "frame_object.h"

typedef struct {
    FrameObject* frame;
    uint8_t* pc;
    void* fun;
} pvm;

int pvm_run(pvm* vm, CodeObject* code);
pvm* vm_init();
void vm_destroy(pvm* vm);

#endif
