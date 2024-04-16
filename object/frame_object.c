#include "frame_object.h"

#include <stdlib.h>

static void frame_destr(Object* obj);


TypeObject type_frame = {
    .name = "frame",
    .destr = frame_destr,
};


Object* init_frame(Object* code) {
    FrameObject* frame = malloc(sizeof(FrameObject));
    frame->base.refcnt = 1;
    frame->base.type = &type_frame;
    frame->code = code;
    return (Object*) frame;
}

static void frame_destr(Object* obj) {
    free(obj);
}
