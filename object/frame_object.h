#ifndef FRAME_OBJECT_H
#define FRAME_OBJECT_H

#include "object.h"
#include "code_object.h"
#include "dict_object.h"
#include "func_object.h"

extern TypeObject type_frame;

typedef struct FrameObject FrameObject;

struct FrameObject {
    Object base;
    FrameObject* prev_frame;
    FrameObject* next_frame;
    CodeObject* code;
    FuncObject* func;
    DictObject* locals;

    uint8_t* pc;
    Object** sp;

    Object* localsplus[];
};

FrameObject* init_frame(CodeObject* code);
FrameObject* frame_new(FuncObject* func);

#endif
