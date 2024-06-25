#include "list_object.h"
#include "str_object.h"
#include "func_object.h"
#include "dict_object.h"
#include "none_object.h"
#include "debugger.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

static Object* list_str(Object* obj);
static void list_destr(Object* obj);
static void list_expand_size(ListObject* o);
static Object* list_get_attr(Object* owner, Object* attr);

TypeObject type_list = {
    .name = "list",
    .str  = list_str,
    .destr = list_destr,
    .get_attr = list_get_attr,
    .dict = NULL,
};

Object* list_new(int n) {
    ListObject* ret = malloc(sizeof(ListObject));
    n = n > 4 ? n : 4;
    ret->base.refcnt = 1;
    ret->base.type = &type_list;
    ret->len = 0;
    ret->capacity = n;
    ret->items = malloc(n * sizeof(Object*));
    memset(ret->items, 0, n * sizeof(Object*));
    return (Object*) ret;
}

static Object* list_str(Object* obj) {
    assert(obj->type == &type_list);
    ListObject* l = (ListObject*) obj;
    uint8_t tmp[2048];
    int tmp_index = 0;
    tmp[0] = '[';
    tmp_index += 1;
    for (int i = 0; i < l->len; i++) {
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

    return string_new(tmp, tmp_index);
}

static void list_destr(Object* obj) {
    assert(obj->type == &type_list);
    ListObject* l = (ListObject*) obj;

    for (int i = 0; i < l->len; i++) {
        DECREF(l->items[i]);
    }

    free(l->items);
    free(l);
}

int list_set(Object* list, int index, Object* o) {
    assert(list->type == &type_list);
    ListObject* l = (ListObject*) list;
    assert(index < l->len);
    DECREF(l->items[index]);
    INCREF(o);
    l->items[index] = o;
    return 0;
}

Object* list_get(Object* list, int index) {
    assert(list->type == &type_list);
    ListObject* l = (ListObject*) list;
    assert(index < l->len);
    return l->items[index];
}

static void list_expand_size(ListObject* l) {
    Object** new_items = malloc(l->len * sizeof(Object*) * 2);
    memcpy(new_items, l->items, l->len * sizeof(Object*));
    memset(new_items + l->len, 0, l->len * sizeof(Object*));
    free(l->items);
    l->items = new_items;
    l->capacity *= 2;
}

void list_append(Object* list, Object* o) {
    assert(list->type == &type_list);
    ListObject* l = (ListObject*) list;
    if (l->len == l->capacity) {
        list_expand_size(l);
    }
    INCREF(o);
    l->items[l->len] = o;
    l->len += 1;
}

static Object* list_get_attr(Object* owner, Object* attr) {
    Object* ret = dict_get((DictObject*) owner->type->dict, attr);
    if (ret) {
        return ret;
    }

    return NULL;
}

static Object* list_append_call(TupleObject* tuple);
static CFuncObject list_append_cf = {
    .base = {
        .refcnt = IMMORTAL_REF,
        .type = &type_cfunc,
    },
    .call = list_append_call,
};

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


static Object* s[256] = {};
static int s_len;

__attribute__((constructor)) static void con() {
    type_list.dict = dict_new();
    s[s_len++] = string_new_cstr("append");
    dict_set((DictObject*) type_list.dict, s[s_len-1], (Object*) &list_append_cf);
}

__attribute__((destructor)) static void des() {
    DECREF(type_list.dict);
    for (int i = 0; i < s_len; i++) {
        DECREF(s[i]);
    }
}
