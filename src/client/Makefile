CFLAGS=-Wall -Werror -Wextra $(shell pkg-config --cflags json-c)
LIBFLAGS=-ldl -lpthread $(shell pkg-config --libs json-c)
DEBUGFLAGS=

all: client

debug: DEBUGFLAGS=-g -DDEBUG
debug: all
CC=gcc $(DEBUGFLAGS)

%.o: %.c %.h
	$(CC) -c -o $@ $< $(CFLAGS)

client: client.o
	$(CC) -o $@ $^ $(LIBFLAGS)

clean:
	rm -f *.o

clean2: clean
	rm -f client

test: debug
	./test.sh

valgrind: debug
	./test.sh valgrind
