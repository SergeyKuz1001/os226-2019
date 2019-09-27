
all : libmalloc.so

CFLAGS=-g

libmalloc.so : malloc.c
	gcc $(CFLAGS) -shared -fpic -o $@ $< -ldl

clean :
	rm -f libmalloc.so hello

test : libmalloc.so hello
	LD_PRELOAD=./$< ./hello
