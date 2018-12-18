CFLAGS += -Wall -Werror -Wextra -std=gnu11 -lpthread

all: stupidlog4c.o

tests/two_threads: tests/two_threads.c stupidlog4c.o
