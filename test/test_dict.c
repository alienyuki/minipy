#include "object.h"
#include "dict_object.h"
#include "str_object.h"
#include "long_object.h"

int main() {
    DictObject* d = (DictObject*) dict_new();
    object_print(1, (Object*) d);
    
    Object* s = string_new((uint8_t*) "test", 4);
    Object* l = long_new(114514);
    dict_set(d, s, l);
    object_print(1, (Object*) d);
    // DECREF(d);
}
