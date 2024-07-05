#ifndef LIST_OBJECT_H
#define LIST_OBJECT_H

#include "object.h"


typedef struct {
    Object base;
    Object** items;
    int len;
    int capacity;
} ListObject;

extern TypeObject type_list;

void list_type_init();
void list_type_destroy();

Object* list_new(int n);
int list_set(Object* list, int index, Object* o);
Object* list_get(Object* list, int index);
void list_append(Object* list, Object* o);
Object* list_pop(Object* list, int index);
// void list_insert(Object* list, int index, Object* o);
int list_size(ListObject* list);

#endif
