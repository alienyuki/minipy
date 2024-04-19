#ifndef FRAME_OBJECT_H
#define FRAME_OBJECT_H

#include "object.h"
#include "code_object.h"

extern TypeObject type_frame;

typedef struct {
    Object base;
    CodeObject* code;
    Object* localsplus[];
} FrameObject;

FrameObject* init_frame(CodeObject* code);

#endif
