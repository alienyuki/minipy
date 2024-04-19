#include "dict_object.h"
#include "str_object.h"

#include <stdlib.h>
#include <string.h>

static Object* dict_str(Object* obj);
static void dict_destr(Object* obj);

TypeObject type_dict = {
    .name = "dict",
    .destr = dict_destr,
    .str = dict_str,
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
    ret->nentries = 8;

    return (Object*) ret;
}


Object* dict_get(DictObject* dict, Object* key) {

    return NULL;
}

Object* dict_pop(DictObject* dict, Object* key) {
    return NULL;
}

void dict_del(DictObject* dict, Object* key) {
    Object* ret = dict_pop(dict, key);
    DECREF(ret);
}


int dict_set(DictObject* dict, Object* key, Object* value) {
    hash_t hash = object_hash(key);
    hash &= dict->nentries - 1;
    DictEntry* e = &(dict->entries[hash]);
    
    if (e->key == NULL) {
        e->key = key;
        e->value = value;
        return 0;
    }

    /*
        TODO: eq
        if (e->key EQ key) {
            DECREF(e->value);
            e->value = value;
            return 0;
        }
    */

    while (e->next != NULL) {
    /*
        TODO: eq
        if (e->key EQ key) {
            DECREF(e->value);
            e->value = value;
            return 0;
        }
    */
        e = e->next;
    }

    e->next = malloc(sizeof(DictEntry));
    e = e->next;
    e->key = key;
    e->value = value;
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

static void dict_destr(Object* obj) {
    // TODO:
    // 1. destroy items in entries (link-list)
    // 2. destroy entries
    // 3. destroy obj
}

