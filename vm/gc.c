#include "gc.h"
#include "debugger.h"

#include <stdlib.h>
#include <stdio.h>

static gc_head gc_dummy_head = {
    .next = &gc_dummy_head,
    .prev = &gc_dummy_head,
};

static gc_head* gc_list = &gc_dummy_head;

#define GC_STATUS_COLLECTING (1 << 0)
#define IS_COLLECTING(head) (((head)->gc_status & GC_STATUS_COLLECTING) == 1)
#define SET_COLLECTING(head) (((head)->gc_status) |= GC_STATUS_COLLECTING)

void* gc_malloc(int size) {
    gc_head* ret = malloc(size + sizeof(gc_head));

    ret->next = gc_list->next;
    ret->prev = gc_list;
    gc_list->next->prev = ret;
    gc_list->next = ret;

    ret->gc_status  = 0;
    ret->shadow_ref = 0;

    return GC2OBJ(ret);
}

void gc_free(void* p) {
    gc_head* a = OBJ2GC(p);
    a->prev->next = a->next;
    a->next->prev = a->prev;
    free(a);
}

static int decrease_ref(Object* o, void* arg) {
    if (!IS_COLLECTING(OBJ2GC(o))) {
        SET_COLLECTING(OBJ2GC(o));
        OBJ2GC(o)->shadow_ref -= 1;
        return 0;
    }
    return 1;
}

void gc() {
    gc_head* p = gc_list;

    p = p->next;
    // set shadow_ref
    while (p != gc_list) {
        p->shadow_ref = GC2OBJ(p)->refcnt;
        p->gc_status = 0;
        object_print(1, GC2OBJ(p));
        printf(" %d\n", p->shadow_ref);
        p = p->next;
    }
    // Traverse and decrease shadow_ref.
    p = gc_list->next;
    while (p != gc_list) {
        Object* o = GC2OBJ(p);
        o->type->traverse(o, decrease_ref, o);
        p = p->next;
    }

    // If shadow_ref is not decreased to 0, then that is a root obj.
    // else, it is in a cycle or referenced by root obj.
    // Split link list into two part: root objs and maybe_unreachable.
    gc_head dummy_root_obj = {
        .next = &dummy_root_obj,
        .prev = &dummy_root_obj,
        .gc_status = 0,
        .shadow_ref = 0,
    };
    gc_head* root_obj = &dummy_root_obj;

    gc_head dummy_unreachable = {
        .next = &dummy_unreachable,
        .prev = &dummy_unreachable,
        .gc_status = 0,
        .shadow_ref = 0,
    };
    gc_head* unreachable = &dummy_unreachable;

    p = p->next;
    while (p != gc_list) {
        gc_head* q = p->next;
        // printf("p->shadow_ref: %d\n", p->shadow_ref);
        if (p->shadow_ref == 0) {
            p->next = unreachable->next;
            p->prev = unreachable;
            unreachable->next->prev = p;
            unreachable->next = p;
        } else {
            p->next = root_obj->next;
            p->prev = root_obj;
            root_obj->next->prev = p;
            root_obj->next = p;            
        }
        p = q;
    }

    printf("unreachable: \n");
    gc_head* un = unreachable->next;
    while (un != unreachable) {
        object_print(1, GC2OBJ(un));
        printf("\n");
        un = un->next;
    }
    printf("end unreachable\n");

    // Traverse roots and mark all the accessed elements as reachable.
    // Now the remaining unreachable is really unreachable.
    // Use TypeObject::clear to break reference cycles
    // TypeObject::clear should ensure that itself is destroyed but refcnt
    // is not changed and it is safe to call TypeObject::destr.

    // recover the gc_list
    if (root_obj->next == root_obj) {
        gc_list->next = gc_list;
        gc_list->prev = gc_list;
    } else {
        gc_list->next = root_obj->next;
        gc_list->prev = root_obj->prev;
        gc_list->next->prev = gc_list;
        gc_list->prev->next = gc_list;
    }
}
