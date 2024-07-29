#ifndef VM_H
#define VM_H

#include "object.h"
#include "code_object.h"
#include "frame_object.h"
#include "dict_object.h"

typedef struct break_point {
    enum {
        BP_LINENO,
        BP_FUNCTION,
    } type;

    union {
        struct {
            char func[32];
            int  bc;
        } f;
    };
} break_point;


typedef struct {
    FrameObject* frame;
    DictObject* globals;
    uint8_t* pc;
    Object** sp;

    int instr_step;
    char last_dbg_cmd[256];

    break_point breakpoints[16];
    int bpn;
} pvm;

int pvm_run(pvm* vm, CodeObject* code);
pvm* vm_init(int default_dbg);
void vm_destroy(pvm* vm);

#endif
