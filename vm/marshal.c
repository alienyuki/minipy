#include "marshal.h"
#include "str_object.h"
#include "tuple_object.h"
#include "long_object.h"
#include "none_object.h"
#include "list_object.h"
#include "code_object.h"
#include "bool_object.h"
#include "debugger.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>


#define TYPE_CODE 'c'
#define TYPE_STRING 's'
#define TYPE_SMALL_TUPLE ')'
#define TYPE_INT 'i'
#define TYPE_NONE 'N'
#define TYPE_TRUE 'T'
#define TYPE_FALSE 'F'
#define TYPE_SHORT_ASCII 'z'
#define TYPE_SHORT_ASCII_INTERNED 'Z'
#define TYPE_REF 'r'


typedef struct {
    uint8_t  buffer[4096];
    uint8_t* start;
    uint8_t* end;
    ListObject* refs;
} pyc_file;


static uint8_t r_byte(pyc_file* f) {
    uint8_t* p = f->start;
    uint8_t ret = p[0];
    f->start += 1;
    return ret;
}


static int r_long(pyc_file* f) {
    uint8_t* p = f->start;
    int ret = p[0] + (p[1] << 8) + (p[2] << 16) + (p[3] << 24);
    f->start += 4;
    return ret;
}

static void* r_object(pyc_file* f) {
    uint8_t ref_type = *f->start;
    f->start++;
    Object* ret = NULL;

    // TODO: What does ref do?
    uint8_t ref  = ref_type & 0x80;
    uint8_t type = ref_type & ~0x80;

    switch (type) {
    case TYPE_SHORT_ASCII_INTERNED:
    // fall through
    case TYPE_SHORT_ASCII: {
        int len = r_byte(f);
        uint8_t* s = f->start;
        f->start += len;
        ret = string_new(s, len);
        break;
    }

    case TYPE_NONE: {
        ret = none_new();
        break;
    }

    case TYPE_TRUE: {
        ret = true_new();
        break;
    }

    case TYPE_FALSE: {
        ret = false_new();
        break;
    }

    case TYPE_REF: {
        int n = r_long(f);

        ret = list_get((Object*) f->refs, n);

        INCREF(ret);
        break;
    }

    case TYPE_INT: {
        int len = r_long(f);
        ret = long_new(len);
        break;
    }

    case TYPE_SMALL_TUPLE: {
        int len = r_byte(f);
        ret = tuple_new(len);

        // For tuple, list and dict, you should add containers first in the
        // ref list, then add the items.
        if (ref) {
            list_append((Object*) f->refs, ret);
        }

        for (int i = 0; i < len; i++) {
            Object* o = (Object*) r_object(f);
            tuple_set(ret, i, o);
        }

        return ret;
    }

    case TYPE_STRING: {
        int len = r_long(f);
        uint8_t* s = f->start;
        f->start += len;
        ret = string_new(s, len);
        break;
    }

    case TYPE_CODE: {
        int argcount        = r_long(f);
        int posonlyargcount = r_long(f);
        int kwonlyargcount  = r_long(f);
        int stacksize       = r_long(f);
        int flags           = r_long(f);

        // Figure out why list_append a none
        __attribute__((unused))
        int reserve_idx;
        if (ref) {
            // reserve a place for the final generate code object
            reserve_idx = f->refs->len;
            list_append((Object*) f->refs, none_new());
        }

        void* code = r_object(f);
        void* consts = r_object(f);
        void* names = r_object(f);
        void* localsplusnames = r_object(f);
        void* localspluskinds = r_object(f);
        void* filename = r_object(f);
        void* name = r_object(f);
        void* qualname = r_object(f);
        int firstlineno = r_long(f);
        void* linetable = r_object(f);
        void* exceptiontable = r_object(f);

        CodeCons code_con = {
            .filename = filename,
            .name     = name,
            .qualname = qualname,
            .flags    = flags,
            .code = code,
            .firstlineno = firstlineno,
            .linetable = linetable,
            .consts = consts,
            .names = names,
            .localsplusnames = localsplusnames,
            .localspluskinds = localspluskinds,
            .argcount = argcount,
            .posonlyargcount = posonlyargcount,
            .kwonlyargcount = kwonlyargcount,
            .stacksize = stacksize,
            .exceptiontable = exceptiontable,
        };

        CodeObject* code_obj = init_code(&code_con);
        // insert code_obj to ref

        ret = (Object*) code_obj;

        DECREF(code);
        DECREF(consts);
        DECREF(names);
        DECREF(localsplusnames);
        DECREF(localspluskinds);
        DECREF(filename);
        DECREF(name);
        DECREF(qualname);
        DECREF(linetable);
        DECREF(exceptiontable);

        break;
    }

    default: {
        TODO("r_object: type 0x%x ('%c') is not implemented yet\n", type, type);
    }
    }

    if (ref) {
        list_append((Object*) f->refs, ret);
    }
    return ret;
}


void* unmarshal_pyc(const char* filename) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        printf("error: open\n");
        exit(1);
    }

    pyc_file f;
    f.refs = (ListObject *) list_new(0);
    int n = read(fd, f.buffer, 4096);
    if (n == -1) {
        printf("error: open\n");
        exit(1);
    }

    if (n == 4096) {
        printf("error: %s too long\n", filename);
        exit(1);
    }

    f.start = f.buffer;
    f.end = f.start + n;

    r_long(&f);
    r_long(&f);
    r_long(&f);
    r_long(&f);

    Object* code = r_object(&f);
    DECREF(f.refs);
    return code;
}
