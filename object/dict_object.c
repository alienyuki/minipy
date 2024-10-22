#include "dict_object.h"
#include "str_object.h"
#include "bool_object.h"
#include "none_object.h"
#include "tuple_object.h"
#include "func_object.h"
#include "debugger.h"
#include "gc.h"

#include <stdlib.h>
#include <string.h>

static Object* dict_str(Object* obj);
static void dict_destr(Object* obj);
static void dict_clear(Object* obj);
static Object* dict_get_attr(Object* owner, Object* attr);
static int dict_traverse(Object* o, tr_visit visit, void* args);

static Object* dict_get_sub(Object* dict, Object* k);
static int     dict_set_sub(Object* dict, Object* k, Object* v);

static map_methods dict_seq_methods = {
    .get_sub = dict_get_sub,
    .set_sub = dict_set_sub,
};

TypeObject type_dict = {
    .name = "dict",
    .destr = dict_destr,
    .str = dict_str,
    .get_attr = dict_get_attr,
    .traverse = dict_traverse,
    .clear = dict_clear,
    .map = &dict_seq_methods,
    .flag = TYPE_FLAG_GC,
};

struct DictEntry {
    Object* key;
    Object* value;
    DictEntry* next;
};


Object* dict_new() {
    const static int INITAIL_SIZE = 8;
    DictObject* ret = gc_malloc(sizeof(DictObject));
    DictEntry* entries = malloc(sizeof(DictEntry) * INITAIL_SIZE);
    memset(entries, 0, sizeof(DictEntry) * INITAIL_SIZE);
    ret->entries = entries;
    ret->base.refcnt = 1;
    ret->base.type = &type_dict;
    ret->nitems = 0;
    ret->nentries = INITAIL_SIZE;

    return (Object*) ret;
}


Object* dict_get(DictObject* dict, Object* key) {
    hash_t hash = object_hash(key);
    hash &= dict->nentries - 1;
    DictEntry* e = &(dict->entries[hash]);

    if (e->key == NULL) {
        return NULL;
    }

    while (e->next != NULL) {
        if (object_compare(e->key, key, CMP_EQ) == true_new()) {
            return e->value;
        }
        e = e->next;
    }

    if (object_compare(e->key, key, CMP_EQ) == true_new()) {
        return e->value;
    }

    return NULL;
}

Object* dict_pop(DictObject* dict, Object* key) {
    hash_t hash = object_hash(key);
    hash &= dict->nentries - 1;
    DictEntry* e = &(dict->entries[hash]);

    if (e->key == NULL) {
        return NULL;
    }

    if (object_compare(e->key, key, CMP_EQ) == true_new()) {
        Object* ret = e->value;
        DECREF(e->key);

        if (e->next != NULL) {
            e->key = e->next->key;
            e->value = e->next->value;
            DictEntry* tmp = e->next;
            e->next = tmp->next;
            free(tmp);
        } else {
            e->key = NULL;
            e->value = NULL;
        }
        dict->nitems -= 1;
        return ret;
    }

    while (e->next != NULL) {
        DictEntry* cur = e;
        e = e->next;
        if (object_compare(e->key, key, CMP_EQ) == true_new()) {
            Object* ret = e->value;
            DECREF(e->key);
            cur->next = e->next;
            free(e);
            dict->nitems -= 1;
            return ret;
        }
    }

    return NULL;
}

void dict_del(DictObject* dict, Object* key) {
    Object* ret = dict_pop(dict, key);
    if (ret != NULL) {
        DECREF(ret);
    }
}


int dict_set(DictObject* dict, Object* key, Object* value) {
    hash_t hash = object_hash(key);
    hash &= dict->nentries - 1;
    DictEntry* e = &(dict->entries[hash]);

    if (e->key == NULL) {
        INCREF(key);
        INCREF(value);
        e->key = key;
        e->value = value;
        dict->nitems += 1;
        return 0;
    }

    while (e->next != NULL) {
        if (object_compare(e->key, key, CMP_EQ) == true_new()) {
            INCREF(value);
            DECREF(e->value);
            e->value = value;
            return 0;
        }
        e = e->next;
    }

    if (object_compare(e->key, key, CMP_EQ) == true_new()) {
        INCREF(value);
        DECREF(e->value);
        e->value = value;
        return 0;
    }

    // maybe use pool instead of directerly malloc to reduce fragmentation
    e->next = malloc(sizeof(DictEntry));
    e = e->next;
    INCREF(key);
    INCREF(value);
    e->key = key;
    e->value = value;
    dict->nitems += 1;
    e->next = NULL;

    return 0;
}

static Object* str_dict[64];
static int str_dict_idx;


static Object* dict_str(Object* obj) {
    DictObject* dict = (DictObject*) obj;

    if (dict->entries == NULL) {
        return string_new_cstr("{gcing dict}");
    }

    for (int i = 0; i < str_dict_idx; i++) {
        if (str_dict[i] == obj) {
            return string_new_cstr("{...}");
        }
    }
    str_dict[str_dict_idx++] = obj;

    uint8_t tmp[4096];
    int ti = 0;
    tmp[ti] = '{';
    ti += 1;

    for (int i = 0; i < dict->nentries; i++) {
        DictEntry* e = &(dict->entries[i]);
        if (e->key == NULL) {
            continue;
        }

        while (e) {
            StrObject* sk = (StrObject*) e->key->type->str(e->key);
            StrObject* sv = (StrObject*) e->value->type->str(e->value);

            memcpy(tmp + ti, sk->str, sk->size);
            ti += sk->size;

            tmp[ti] = ':';
            tmp[ti + 1] = ' ';
            ti += 2;

            memcpy(tmp + ti, sv->str, sv->size);
            ti += sv->size;

            tmp[ti] = ',';
            tmp[ti + 1] = ' ';
            ti += 2;

            e = e->next;
            DECREF(sk);
            DECREF(sv);
        }
    }

    if (ti != 1) {
        ti -= 2;
    }

    tmp[ti] = '}';
    ti += 1;

    str_dict_idx -= 1;
    return string_new(tmp, ti);
}

static Object* dict_get_sub(Object* dict, Object* k) {
    Object* ret = dict_get((DictObject*) dict, k);
    if (ret) {
        return ret;
    }
    panic("dict key error");
}

static int dict_set_sub(Object* dict, Object* k, Object* v) {
    return dict_set((DictObject*) dict, k, v);
}

static int dict_traverse(Object* o, tr_visit visit, void* args) {
    DictObject* dict = (DictObject*) o;
    for (int i = 0; i < dict->nentries; i++) {
        DictEntry* e = &(dict->entries[i]);
        if (e->key == NULL) {
            continue;
        }
        while (e) {
            visit(e->key, args);
            visit(e->value, args);
            e = e->next;
        }
    }

    return 0;
}

static void dict_destr(Object* obj) {
    DictObject* dict = (DictObject*) obj;

    if (dict->entries == NULL) {
        return;
    }

    for (int i = 0; i < dict->nentries; i++) {
        DictEntry* e1 = &dict->entries[i];
        DictEntry* e2 = e1->next;
        if (e1->key != NULL) {
            DECREF(e1->key);
            DECREF(e1->value);
        }
        while (e2 != NULL) {
            e1 = e2;
            e2 = e2->next;
            DECREF(e1->key);
            DECREF(e1->value);
            free(e1);
        }
    }

    free(dict->entries);
    gc_free(dict);
}

static void dict_clear(Object* obj) {
    DictObject* dict = (DictObject*) obj;

    DictEntry* entries = dict->entries;
    dict->entries = NULL;
    int n = dict->nentries;
    dict->nentries = -1;

    for (int i = 0; i < n; i++) {
        DictEntry* e1 = &entries[i];
        DictEntry* e2 = e1->next;
        if (e1->key != NULL) {
            DECREF(e1->key);
            DECREF(e1->value);
        }
        while (e2 != NULL) {
            e1 = e2;
            e2 = e2->next;
            DECREF(e1->key);
            DECREF(e1->value);
            free(e1);
        }
    }

    free(entries);
}

static Object* dict_get_attr(Object* owner, Object* attr) {
    Object* ret = dict_get((DictObject*) owner->type->dict, attr);
    if (ret) {
        return ret;
    }

    return NULL;
}

#define DICT_CF(method) \
static Object* dict_##method##_call(TupleObject* tuple);    \
static CFuncObject dict_##method##_cf = {                   \
    .base = {                                               \
        .refcnt = IMMORTAL_REF,                             \
        .type = &type_cfunc,                                \
    },                                                      \
    .call = dict_##method##_call,                           \
}

DICT_CF(pop);
static Object* dict_pop_call(TupleObject* tuple) {
    DictObject* dict = (DictObject*) tuple_get(tuple, 0);
    Object* key = tuple_get(tuple, 1);
    Object* ret = dict_pop(dict, key);
    if (ret) {
        return ret;
    }

    panic("dict key error");
}

static Object* s[256] = {};
static int s_len;

#define INSERT_CF(method) \
    s[s_len] = string_new_cstr(#method);            \
    dict_set((DictObject*) type_dict.dict,          \
        s[s_len++], (Object*) &dict_##method##_cf)  \


void dict_type_init() {
    type_dict.dict = dict_new();
    INSERT_CF(pop);
}

void dict_type_destroy() {
    DECREF(type_dict.dict);
    for (int i = 0; i < s_len; i++) {
        DECREF(s[i]);
    }
    s_len = 0;
}
