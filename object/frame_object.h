#ifndef FRAME_OBJECT_H
#define FRAME_OBJECT_H

#include "object.h"

extern TypeObject type_frame;

typedef struct {
    Object base;
    Object* code;
    Object* localsplus[];
} FrameObject;

Object* init_frame(Object* code);

#endif
