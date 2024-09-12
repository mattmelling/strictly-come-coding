.DEFAULT: brc

CFLAGS ?= -lm
CFLAGS_DEBUG ?= -O0 -g -DDEBUG

ifeq ($(DEBUG),1)
CFLAGS := $(CFLAGS) $(CFLAGS_DEBUG)
endif

%.o: %.c %.h
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -f *.o test_parser test_hashtable brc_sequential_read

# solutions

brc_sequential_read: brc_sequential_read.c parser.o hashtable.o hash.o
	$(CC) $(CFLAGS) -o $@ $^

# tests

test_parser: test_parser.c parser.o debug.h
	$(CC) $(CFLAGS) -o $@ $^

test_hashtable: test_hashtable.c hash.o hashtable.o parser.o debug.h
	$(CC) $(CFLAGS) -o $@ $^
