#include "frame_object.h"
#include "code_object.h"

#include <stdlib.h>

static void frame_destr(Object* obj);


TypeObject type_frame = {
    .name = "frame",
    .destr = frame_destr,
};


Object* init_frame(Object* code) {
    int stack_size = ((CodeObject*) code)->stack_size;
    FrameObject* frame = malloc(sizeof(FrameObject) + stack_size * sizeof(Object*));
    frame->base.refcnt = 1;
    frame->base.type = &type_frame;
    frame->code = code;
    return (Object*) frame;
}

static void frame_destr(Object* obj) {
    free(obj);
}
