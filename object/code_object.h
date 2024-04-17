#ifndef CODE_OBJECT_H
#define CODE_OBJECT_H

#include "object.h"
#include <stdint.h>


typedef struct {
    Object base;
    int size;
    Object* consts;
    int stack_size;
    Object* localsplusnames;
    uint8_t bytecodes[];
} CodeObject;

extern TypeObject type_code;

typedef struct {
    /* metadata */
    Object *filename;
    Object *name;
    Object *qualname;
    int flags;

    /* the code */
    Object *code;
    int firstlineno;
    Object *linetable;

    /* used by the code */
    Object *consts;
    Object *names;

    /* mapping frame offsets to information */
    Object *localsplusnames;  // Tuple of strings
    Object *localspluskinds;  // Bytes object, one byte per variable

    /* args (within varnames) */
    int argcount;
    int posonlyargcount;
    // XXX Replace argcount with posorkwargcount (argcount - posonlyargcount).
    int kwonlyargcount;

    /* needed to create the frame */
    int stacksize;

    /* used by the eval loop */
    Object *exceptiontable;
} CodeCons;

CodeObject* init_code(CodeCons* CodeCons);

#endif