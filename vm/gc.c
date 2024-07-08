#include "gc.h"
#include "debugger.h"

#include <stdlib.h>
#include <stdio.h>

static gc_head gc_dummy_head = {
    .next = &gc_dummy_head,
    .prev = &gc_dummy_head,
};

static gc_head* gc_list = &gc_dummy_head;

static int gc_cnt;

void* gc_malloc(int size) {
    gc_cnt += 1;
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
    gc_cnt -= 1;

    gc_head* a = OBJ2GC(p);
    a->prev->next = a->next;
    a->next->prev = a->prev;
    free(a);
}

static int decrease_ref(Object* o, void* arg) {
    if ((o->type->flag & TYPE_FLAG_GC) != 0) {
        object_print(1, o);
        OBJ2GC(o)->shadow_ref -= 1;
    }
    return 0;
}

static int add_reachable(Object* o, void* arg) {
    gc_head* root_obj = arg;
    if ((o->type->flag & TYPE_FLAG_GC) != 0) {
        gc_head* p = OBJ2GC(o);
        // add to root objs since it is referenced by root
        if (p->shadow_ref == 0) {
            p->next->prev = p->prev;
            p->prev->next = p->next;
            
            p->next = root_obj;
            p->prev = root_obj->prev;
            root_obj->prev->next = p;
            root_obj->prev = p;

            p->shadow_ref = 1;
        }
    }
    return 0;
}

void gc() {
    gc_head* p = gc_list;

    p = p->next;
    // set shadow_ref
    while (p != gc_list) {
        p->shadow_ref = GC2OBJ(p)->refcnt;
        p->gc_status = 0;
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

    // Traverse roots and mark all the accessed elements as reachable.
    // Now the remaining unreachable is really unreachable.

    gc_head* r = root_obj->next;
    while (r != root_obj) {
        Object* o = GC2OBJ(r);
        o->type->traverse(o, add_reachable, root_obj);
        r = r->next;
    }

    gc_head* un = unreachable->next;
    while (un != unreachable) {
        un = un->next;
    }

    // Use TypeObject::clear to break reference cycles
    // TypeObject::clear should ensure that itself is destroyed but refcnt
    // is not changed and it is safe to call TypeObject::destr.
    un = unreachable->next;
    while (un != unreachable) {
        Object* o = GC2OBJ(un);
        o->type->clear(o);
        un = un->next;
    }

    un = unreachable->next;
    while (un != unreachable) {
        gc_head* p = un->next;
        gc_free(GC2OBJ(un));
        un = p;
    }

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

void gc_complete_assert() {
    if (gc_cnt != 0) {
        panic("gc is not complete");
    }
}
