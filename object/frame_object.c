#include "frame_object.h"
#include "code_object.h"

#include <stdlib.h>

static void frame_destr(Object* obj);


TypeObject type_frame = {
    .name = "frame",
    .destr = frame_destr,
};


FrameObject* init_frame(CodeObject* code) {
    int stack_size = code->stack_size;
    FrameObject* frame = malloc(sizeof(FrameObject) + stack_size * sizeof(Object*));
    frame->base.refcnt = 1;
    frame->base.type = &type_frame;
    frame->code = code;
    return frame;
}

static void frame_destr(Object* obj) {
    free(obj);
}
