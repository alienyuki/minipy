#include "marshal.h"
#include "str_object.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>


#define TYPE_CODE 'c'
#define TYPE_STRING 's'


typedef struct {
    uint8_t  buffer[4096];
    uint8_t* start;
    uint8_t* end;
} pyc_file;


static int r_long(pyc_file* f) {
    uint8_t* p = f->start;
    int ret = p[0] + (p[1] << 8) + (p[2] << 16) + (p[3] << 24);
    f->start += 4;
    return ret;
}

static void* r_object(pyc_file* f) {
    uint8_t ref_type = *f->start;
    f->start++;

    // TODO: What does ref do?
    // uint8_t ref = ref_type | 0x80;
    uint8_t type = ref_type & ~0x80;

    switch (type) {
    case TYPE_STRING: {
        printf("type string\n");
        int len = r_long(f);
        uint8_t* s = f->start;
        f->start += len;
        Object* ret = string_new(s, len);
        return ret;
        break;
    }

    case TYPE_CODE: {
        printf("type code\n");
        int argcount        = r_long(f);
        int posonlyargcount = r_long(f);
        int kwonlyargcount  = r_long(f);
        int stacksize       = r_long(f);
        int flags           = r_long(f);

        printf("argcount:        %d\n", argcount);
        printf("posonlyargcount: %d\n", posonlyargcount);
        printf("kwonlyargcount:  %d\n", kwonlyargcount);
        printf("stacksize:       %d\n", stacksize);
        printf("flags:           %d\n", flags);

        // Layout of code object
        // fill these one by one
        // void* code          = r_object(f);
        // void* consts        = r_object(f);
        // void* names         = r_object(f);
        // void* localsplusnames = r_object(f);
        // void* localspluskinds = r_object(f);
        // void* filename      = r_object(f);
        // void* name          = r_object(f);
        // void* qualname      = r_object(f);
        // int firstlineno     = r_long(f);
        // void* linetable     = r_object(f);
        // void* exceptiontable = r_object(f);

        void* code = r_object(f);
        object_print(1, code);
        void* consts = r_object(f);
        printf("%p\n", consts);

        break;
    }

    default: {
        printf("r_object: type 0x%x ('%c') is not implemented yet\n", type, type);
        exit(1);
    }
    }

    return NULL;
}


void unmarshal_pyc(const char* filename) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        printf("error: open\n");
        exit(1);
    }

    pyc_file f;
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

    printf("%x\n", r_long(&f));
    printf("%x\n", r_long(&f));
    printf("%x\n", r_long(&f));
    printf("%x\n", r_long(&f));
    r_object(&f);
}
