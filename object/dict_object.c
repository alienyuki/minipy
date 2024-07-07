#include "dict_object.h"
#include "str_object.h"
#include "bool_object.h"
#include "none_object.h"
#include "debugger.h"

#include <stdlib.h>
#include <string.h>

static Object* dict_str(Object* obj);
static void dict_destr(Object* obj);

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
    .map = &dict_seq_methods,
};

struct DictEntry {
    Object* key;
    Object* value;
    DictEntry* next;
};


Object* dict_new() {
    const static int INITAIL_SIZE = 8;
    DictObject* ret = malloc(sizeof(DictObject));
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

static Object* dict_str(Object* obj) {
    DictObject* dict = (DictObject*) obj;
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


static void dict_destr(Object* obj) {
    DictObject* dict = (DictObject*) obj;

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
    free(dict);
}
