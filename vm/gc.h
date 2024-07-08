#ifndef GC_H
#define GC_H

#include "object.h"

typedef struct gc_head gc_head;

struct gc_head {
    gc_head* prev;
    gc_head* next;
    int shadow_ref;
    int gc_status;
};

#define GC2OBJ(p) ((Object*)  (((char*) p) + sizeof(gc_head)))
#define OBJ2GC(p) ((gc_head*) (((char*) p) - sizeof(gc_head)))

void* gc_malloc(int size);
void gc_free(void* p);
void gc();
void gc_complete_assert();

#endif
