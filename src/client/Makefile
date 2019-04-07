CFLAGS=-Wall -Werror -Wextra
LIBFLAGS=-ldl -lpthread
DEBUGFLAGS=

all: rsfind

debug: DEBUGFLAGS=-g -DDEBUG
debug: all
CC=gcc $(DEBUGFLAGS)

%.o: %.c %.h
	$(CC) -c -o $@ $< $(CFLAGS)

rsfind: src/rsfind.o src/utils.o src/optionRS.o src/listDir.o src/image.o src/print.o src/exec.o src/regex.o
	$(CC) -o $@ $^ $(LIBFLAGS)

clean:
	rm -f src/*.o

clean2: clean
	rm -f rsfind

test: debug
	./test.sh

valgrind: debug
	./test.sh valgrind