#include "object.h"
#include "dict_object.h"
#include "str_object.h"
#include "long_object.h"

int main() {
    DictObject* d = (DictObject*) dict_new();
    object_print(1, (Object*) d);
    
    Object* s1 = string_new((uint8_t*) "test", 4);
    Object* s2 = string_new((uint8_t*) "this", 4);
    Object* l1 = long_new(114514);
    Object* l2 = long_new(1919810);
    Object* l3 = long_new(8);
    dict_set(d, s1, l1);
    object_print(1, (Object*) d);

    dict_set(d, s2, l2);
    object_print(1, (Object*) d);

    dict_set(d, s2, l3);
    object_print(1, (Object*) d);

    DECREF(s1);
    DECREF(s2);
    DECREF(l1);
    DECREF(l2);
    DECREF(l3);
    DECREF(d);
}
