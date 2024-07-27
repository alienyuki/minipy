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

    int instr_step;
    char last_dbg_cmd[256];
} pvm;

int pvm_run(pvm* vm, CodeObject* code);
pvm* vm_init(int default_dbg);
void vm_destroy(pvm* vm);

#endif
