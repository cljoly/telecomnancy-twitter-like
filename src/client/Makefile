CFLAGS=-Wall -Werror -Wextra
LIBFLAGS=-ldl -lpthread
DEBUGFLAGS=

all: client

debug: DEBUGFLAGS=-g -DDEBUG
debug: all
CC=gcc $(DEBUGFLAGS)

%.o: %.c %.h
	$(CC) -c -o $@ $< $(CFLAGS)

rsfind: src/client.o
	$(CC) -o $@ $^ $(LIBFLAGS)

clean:
	rm -f *.o

clean2: clean
	rm -f client

test: debug
	./test.sh

valgrind: debug
	./test.sh valgrind
