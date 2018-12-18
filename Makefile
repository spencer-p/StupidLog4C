CFLAGS += -Wall -Werror -Wextra

all: stupidlog4c.so stupidlog4c.a

stupidlog4c.so: stupidlog4c.o
	$(CC) $(CFLAGS) -shared -o $@ $^

stupidlog4c.a: stupidlog4c.o
	ar rcs $@ $^

stupidlog4c.o: stupidlog4c.c stupidlog4c.h
	$(CC) $(CFLAGS) -fPIC -c -o $@ $<

tests/two_threads: tests/two_threads.c stupidlog4c.o
tests/two_threads: CFLAGS += -lpthread

.PHONY: clean
clean:
	rm -rf stupidlog4c.o stupidlog4c.so stupidlog4c.a
	find -executable -type f -delete
