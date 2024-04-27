#ifndef FUNC_OBJECT_H
#define FUNC_OBJECT_H

#include "object.h"
#include "code_object.h"

extern TypeObject type_func;

typedef struct {
    Object base;
    CodeObject* code;
} FuncObject;

Object* func_new(CodeObject* code);

#endif
