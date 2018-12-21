CFLAGS += -Wall -Werror -Wextra -lpthread

all: stupidlog4c.o

tests/two_threads: tests/two_threads.c stupidlog4c.o

clean:
	find -name *.o -delete
	find -executable -type f -delete
