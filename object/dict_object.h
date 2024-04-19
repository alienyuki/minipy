#ifndef DICT_OBJECT_H
#define DICT_OBJECT_H

#include "object.h"
#include "list_object.h"

typedef struct DictEntry DictEntry;


typedef struct {
    Object base;
    DictEntry* entries;
    int nitems;
    int nentries;
} DictObject;

extern TypeObject type_dict;

Object* dict_new();
Object* dict_get(DictObject* dict, Object* key);
void dict_del(DictObject* dict, Object* key);
Object* dict_pop(DictObject* dict, Object* key);
int dict_set(DictObject* dict, Object* key, Object* value);

#endif
