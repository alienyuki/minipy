#ifndef FRAME_OBJECT_H
#define FRAME_OBJECT_H

#include "object.h"
#include "code_object.h"
#include "dict_object.h"

extern TypeObject type_frame;

typedef struct FrameObject FrameObject;

struct FrameObject {
    Object base;
    FrameObject* prev_frame;
    FrameObject* next_frame;
    CodeObject* code;
    DictObject* locals;
    Object* localsplus[];
};

FrameObject* init_frame(CodeObject* code);

#endif
