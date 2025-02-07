#include "frame_object.h"
#include "code_object.h"

#include <stdlib.h>
#include <string.h>

static void frame_destr(Object* obj);


TypeObject type_frame = {
    .name = "frame",
    .destr = frame_destr,
};


FrameObject* init_frame(CodeObject* code) {
    int stack_size = code->stack_size + tuple_size(code->localsplusnames);
    FrameObject* frame = malloc(sizeof(FrameObject) + stack_size * sizeof(Object*));
    memset(frame, 0, sizeof(FrameObject) + stack_size * sizeof(Object*));
    frame->base.refcnt = 1;
    frame->base.type = &type_frame;
    frame->code = code;
    INCREF(frame->code);
    frame->locals = NULL;
    return frame;
}

FrameObject* frame_new(FuncObject* func) {
    CodeObject* code = func->code;
    FrameObject* frame = init_frame(code);
    frame->func = func;
    INCREF(func);
    return frame;
}

static void frame_destr(Object* obj) {
    FrameObject* frame = (FrameObject*) obj;
    // Maybe I should free items on frame->localsplus here?

    DECREF(frame->code);
    if (frame->func != NULL) {
        DECREF(frame->func);
    }

    if (frame->locals != NULL) {
        DECREF(frame->locals);
    }

    free(obj);
}
