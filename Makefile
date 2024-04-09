CFLAGS = -g -Wall -Werror

test/test_marshal: test/test_marshal.c vm/marshal.c object/object.c object/str_object.c
	$(CC) -o test/test_marshal $(CFLAGS) -I./vm -I./test -I./object test/test_marshal.c vm/marshal.c object/object.c object/str_object.c
