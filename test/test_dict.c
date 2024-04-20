#include "object.h"
#include "dict_object.h"
#include "str_object.h"
#include "long_object.h"

#include <stdio.h>
#include <string.h>

struct {
    char* s;
    int val;
} a[] = {
    {"aaa", 0},
    {"bbb", 1},
    {"ccc", 1},
    {"ddd", 3},
    {"eee", 2},
    {"fff", 2},
    {"ggg", 5},
    {"hhh", 5},
    {NULL,  0},
};

int main() {
    DictObject* d = (DictObject*) dict_new();
    object_print(1, (Object*) d);
    
    Object* s1 = string_new_cstr("test");
    Object* s2 = string_new_cstr("this");
    Object* s3 = string_new_cstr("yuki");
    Object* s4 = string_new_cstr("alien");
    Object* l1 = long_new(114514);
    Object* l2 = long_new(1919810);
    Object* l3 = long_new(8);
    
    dict_set(d, s1, l1);
    object_print(1, (Object*) d);

    dict_set(d, s4, l1);
    object_print(1, (Object*) d);

    dict_set(d, s3, s4);
    object_print(1, (Object*) d);

    dict_set(d, s2, l2);
    object_print(1, (Object*) d);

    dict_set(d, s2, l3);
    object_print(1, (Object*) d);

    dict_set(d, s1, l3);
    object_print(1, (Object*) d);
    dict_set(d, s2, l3);
    object_print(1, (Object*) d);
    dict_set(d, s3, l3);
    object_print(1, (Object*) d);
    dict_set(d, s4, l3);
    object_print(1, (Object*) d);

    printf("refcnt: %d\n", l3->refcnt);

    Object* v1 = dict_get(d, s1);
    object_print(1, (Object*) v1);
    printf("\nsize: %d\n", d->nitems);

    for (int i = 0; a[i].s != NULL; i++) {
        Object* s = string_new((uint8_t*) a[i].s, strlen(a[i].s));
        Object* v = long_new(a[i].val);
        dict_set(d, s, v);
        DECREF(s);
        DECREF(v);
    }

    Object* pop_v = dict_pop(d, s2);
    object_print(1, (Object*) d);
    object_print(1, (Object*) pop_v);
    DECREF(pop_v);

    dict_del(d, s1);
    object_print(1, (Object*) d);
    printf("\nsize: %d\n", d->nitems);

    DECREF(s1);
    DECREF(s2);
    DECREF(l1);
    DECREF(l2);
    DECREF(l3);
    DECREF(s3);
    DECREF(s4);
    DECREF(d);
}
