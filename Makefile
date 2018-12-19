CFLAGS += -Wall -Werror -Wextra

all: stupidlog4c.so stupidlog4c.a

# Library build targets

stupidlog4c.so: stupidlog4c.o
	$(CC) $(CFLAGS) -shared -o $@ $^

stupidlog4c.a: stupidlog4c.o
	$(AR) rcs $@ $^

stupidlog4c.o: stupidlog4c.c stupidlog4c.h
	$(CC) $(CFLAGS) -fPIC -c -o $@ $<

# Example build targets

%: examples/%.c stupidlog4c.o
	$(CC) $(CFLAGS) -o $@ $^

two_threads: CFLAGS += -lpthread

# Cleanup

.PHONY: clean
clean:
	rm -rf stupidlog4c.o stupidlog4c.so stupidlog4c.a
	find -executable -type f -delete
