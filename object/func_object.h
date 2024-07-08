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
    char* name;
} CFuncObject;

Object* init_builtin_func();
void destroy_builtin_func();

Object* func_new(CodeObject* code);

#endif
