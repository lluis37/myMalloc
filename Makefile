CC = gcc
CFLAGS = -g -Wall -std=c99 -fsanitize=address -O2

all: memtest malloctest memgrind 

memtest: memtest.o mymalloc.o mymalloc.h
	$(CC) $(CFLAGS) memtest.o mymalloc.o -o memtest

malloctest: malloctest.o mymalloc.o mymalloc.h
	$(CC) $(CFLAGS) malloctest.o mymalloc.o -o $@

memgrind: memgrind.o mymalloc.o mymalloc.h
	$(CC) $(CFLAGS) memgrind.o mymalloc.o -o $@

mymalloc.o: mymalloc.c mymalloc.h
	$(CC) -c mymalloc.c

memtest.o: memtest.c mymalloc.h
	$(CC) -c memtest.c

malloctest.o: malloctest.c mymalloc.h
	$(CC) -c malloctest.c

memgrind.o: memgrind.c mymalloc.h
	$(CC) -c memgrind.c

test1: malloctest
	./malloctest 1

test2: malloctest
	./malloctest 2

test3: malloctest
	./malloctest 3

test4: malloctest
	./malloctest 4

test5: malloctest
	./malloctest 5

test6: malloctest
	./malloctest 6

test7: malloctest
	./malloctest 7

test8: malloctest
	./malloctest 8

test9: malloctest
	./malloctest 9

clean:
	rm -f *.o memtest malloctest memgrind
