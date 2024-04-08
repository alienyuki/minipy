CFLAGS = -g -Wall -Werror

test/test_marshal: test/test_marshal.c vm/marshal.c
	$(CC) -o test/test_marshal $(CFLAGS) -I./vm -I./test test/test_marshal.c vm/marshal.c
