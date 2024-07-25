#include "list_object.h"
#include "str_object.h"
#include "func_object.h"
#include "dict_object.h"
#include "none_object.h"
#include "long_object.h"
#include "debugger.h"

#include "gc.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

static Object* list_str(Object* obj);
static void list_destr(Object* obj);
static void list_clear(Object* obj);
static void list_expand_size(ListObject* o);
static Object* list_get_attr(Object* owner, Object* attr);
static int list_traverse(Object* o, tr_visit visit, void* args);
static Object* list_iter(Object* iterable);

static Object* list_get_sub(Object* container, int index);
static int list_set_sub(Object* container, int sub, Object* v);

static void listiter_destr(Object* obj);
static Object* listiter_next(Object* iterable);

static seq_methods list_seq_methods = {
    .get_sub = list_get_sub,
    .set_sub = list_set_sub,
};

TypeObject type_list = {
    .name = "list",
    .str  = list_str,
    .destr = list_destr,
    .get_attr = list_get_attr,
    .dict = NULL,
    .seq = &list_seq_methods,
    .traverse = list_traverse,
    .clear = list_clear,
    .iter = list_iter,
    .flag = TYPE_FLAG_GC,
};

TypeObject type_listiter = {
    .name = "listiter",
    .destr = listiter_destr,
    .itnext = listiter_next,
};

Object* list_new(int n) {
    ListObject* ret = gc_malloc(sizeof(ListObject));
    n = n > 4 ? n : 4;
    ret->base.refcnt = 1;
    ret->base.type = &type_list;
    ret->len = 0;
    ret->capacity = n;
    ret->items = malloc(n * sizeof(Object*));
    memset(ret->items, 0, n * sizeof(Object*));
    return (Object*) ret;
}

static Object* str_list[64];
static int str_list_idx;

static Object* list_str(Object* obj) {
    assert(obj->type == &type_list);
    ListObject* l = (ListObject*) obj;

    if (l->items == NULL) {
        return string_new_cstr("[gcing list]");
    }

    for (int i = 0; i < str_list_idx; i++) {
        if (str_list[i] == obj) {
            return string_new_cstr("[...]");
        }
    }
    str_list[str_list_idx++] = obj;

    uint8_t tmp[2048];
    int tmp_index = 0;
    tmp[0] = '[';
    tmp_index += 1;
    for (int i = 0; i < list_size(l); i++) {
        Object* item = l->items[i];
        StrObject* s = (StrObject*) item->type->str(item);
        memcpy(tmp + tmp_index, s->str, s->size);
        tmp_index += s->size;
        tmp[tmp_index] = ',';
        tmp[tmp_index + 1] = ' ';
        tmp_index += 2;
        DECREF(s);
    }

    if (tmp_index != 1) {
        tmp_index -= 2;
    }

    tmp[tmp_index] = ']';
    tmp_index += 1;

    str_list_idx -= 1;
    return string_new(tmp, tmp_index);
}

static void list_destr(Object* obj) {
    assert(obj->type == &type_list);
    ListObject* l = (ListObject*) obj;

    if (l->items) {
        // NULL means gc_clearing
        for (int i = 0; i < list_size(l); i++) {
            DECREF(l->items[i]);
        }

        free(l->items);
        gc_free(l);
    }
}

static void list_clear(Object* obj) {
    ListObject* l = (ListObject*) obj;
    int size = list_size(l);
    Object** items = l->items;
    l->len = 0;
    l->items = NULL;
    for (int i = 0; i < size; i++) {
        DECREF(items[i]);
    }
    free(items);
}

int list_set(Object* list, int index, Object* o) {
    assert(list->type == &type_list);
    ListObject* l = (ListObject*) list;
    assert(index < list_size(l));
    DECREF(l->items[index]);
    INCREF(o);
    l->items[index] = o;
    return 0;
}

Object* list_get(Object* list, int index) {
    assert(list->type == &type_list);
    ListObject* l = (ListObject*) list;
    assert(index < list_size(l));
    return l->items[index];
}

static void list_expand_size(ListObject* l) {
    Object** new_items = malloc(list_size(l) * sizeof(Object*) * 2);
    memcpy(new_items, l->items, list_size(l) * sizeof(Object*));
    memset(new_items + list_size(l), 0, list_size(l) * sizeof(Object*));
    free(l->items);
    l->items = new_items;
    l->capacity *= 2;
}

void list_append(Object* list, Object* o) {
    assert(list->type == &type_list);
    ListObject* l = (ListObject*) list;
    if (list_size(l) == l->capacity) {
        list_expand_size(l);
    }
    INCREF(o);
    l->items[list_size(l)] = o;
    l->len += 1;
}

Object* list_pop(Object* list, int index) {
    ListObject* l = (ListObject*) list;
    if (list_size(l) == 0) {
        panic("pop from empty list");
    }
    if (index >= list_size(l) || index < 0) {
        panic("Index out of range");
    }

    Object* ret = l->items[index];
    for (int i = index; i < list_size(l) - 1; i++) {
        l->items[i] = l->items[i+1];
    }
    l->len -= 1;
    return ret;
}

static Object* list_get_attr(Object* owner, Object* attr) {
    Object* ret = dict_get((DictObject*) owner->type->dict, attr);
    if (ret) {
        return ret;
    }

    return NULL;
}

static int list_traverse(Object* o, tr_visit visit, void* args) {
    ListObject* l = (ListObject*) o;
    for (int i = 0; i < list_size(l); i++) {
        Object* item = l->items[i];
        if (visit(item, args)) {
            return 1;
        }
    }
    return 0;
}

static Object* list_iter(Object* iterable) {
    ListIterObject* iter = malloc(sizeof(ListIterObject));
    iter->base.type = &type_listiter;
    iter->base.refcnt = 1;
    iter->list = (ListObject*) iterable;
    INCREF(iter->list);
    iter->index = 0;
    return (Object*) iter;
}

static void listiter_destr(Object* obj) {
    ListIterObject* iter = (ListIterObject*) obj;
    DECREF(iter->list);
    free(obj);
}

static Object* listiter_next(Object* iter) {
    ListIterObject* it = (ListIterObject*) iter;
    if (it->index == it->list->len) {
        return NULL;
    }

    it->index += 1;

    Object* next_obj = it->list->items[it->index-1];
    INCREF(next_obj);
    return next_obj;
}

int list_size(ListObject* list) {
    return list->len;
}

static Object* list_get_sub(Object* container, int index) {
    return list_get(container, index);
}

static int list_set_sub(Object* container, int index, Object* v) {
    return list_set(container, index, v);
}

#define LIST_CF(method) \
static Object* list_##method##_call(TupleObject* tuple);    \
static CFuncObject list_##method##_cf = {                   \
    .base = {                                               \
        .refcnt = IMMORTAL_REF,                             \
        .type = &type_cfunc,                                \
    },                                                      \
    .call = list_##method##_call,                           \
}

LIST_CF(append);
static Object* list_append_call(TupleObject* tuple) {
    if (tuple_size(tuple) != 2) {
        panic("index error");
        return NULL;
    }

    Object* list = tuple_get(tuple, 0);
    Object* item = tuple_get(tuple, 1);
    list_append(list, item);

    return (Object*) none_new();
}

LIST_CF(pop);
static Object* list_pop_call(TupleObject* tuple) {
    int sz = tuple_size(tuple);
    if (sz > 2) {
        panic("argument count error");
        return NULL;
    }

    Object* list = tuple_get(tuple, 0);
    int index;
    if (sz == 1) {
        ListObject* l = (ListObject*) list;
        index = list_size(l) - 1;
    } else {
        LongObject* o_idx = (LongObject*) tuple_get(tuple, 1);
        index = o_idx->n;
    }

    Object* ret = list_pop(list, index);
    return ret;
}

static Object* s[256] = {};
static int s_len;

#define INSERT_CF(method) \
    s[s_len] = string_new_cstr(#method);            \
    dict_set((DictObject*) type_list.dict,          \
        s[s_len++], (Object*) &list_##method##_cf)  \


void list_type_init() {
    type_list.dict = dict_new();
    INSERT_CF(append);
    INSERT_CF(pop);
}

void list_type_destroy() {
    DECREF(type_list.dict);
    for (int i = 0; i < s_len; i++) {
        DECREF(s[i]);
    }
    s_len = 0;
}
