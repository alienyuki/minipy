#ifndef FUNC_OBJECT_H
#define FUNC_OBJECT_H

#include "object.h"
#include "code_object.h"

extern TypeObject type_func;
extern TypeObject type_cfunc;

typedef struct {
    Object base;
    CodeObject* code;
} FuncObject;

typedef struct {
    Object base;
    Object* (*call)(TupleObject*);
} CFuncObject;

extern CFuncObject cf_print;

Object* func_new(CodeObject* code);

#endif
