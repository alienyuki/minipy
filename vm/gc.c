#include "gc.h"

#include <stdlib.h>
#include <stdio.h>

static gc_head gc_dummy_head = {
    .next = &gc_dummy_head,
    .prev = &gc_dummy_head,
};

static gc_head* gc_list = &gc_dummy_head;

void* gc_malloc(int size) {
    gc_head* ret = malloc(size + sizeof(gc_head));

    ret->next = gc_list->next;
    ret->prev = gc_list;
    gc_list->next->prev = ret;
    gc_list->next = ret;

    return GC2OBJ(ret);
}

void gc_free(void* p) {
    gc_head* a = OBJ2GC(p);
    a->prev->next = a->next;
    a->next->prev = a->prev;
    free(a);
}

void gc() {
    gc_head* p = gc_list;
    printf("\ngc_list\n");

    p = p->next;
    // set shadow_ref
    while (p != gc_list) {
        object_print(1, GC2OBJ(p));
        printf("\n");
        p->shadow_ref = GC2OBJ(p)->refcnt;
        p = p->next;
    }
    printf("\ngc_list end\n");

    // Traverse and decrease shadow_ref.
    // If shadow_ref is not decreased to 0, then that is a root obj.
    // else, it is in a cycle or referenced by root obj.
    // Split link list into two part: root objs and maybe_unreachable.
    // Traverse roots and mark all the accessed elements as reachable.
    // Now the remaining unreachable is really unreachable.
    // Use TypeObject::clear to break reference cycles
    // TypeObject::clear should ensure that itself is destroyed but refcnt
    // is not changed and it is safe to call TypeObject::destr.
}
