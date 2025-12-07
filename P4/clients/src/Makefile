CC = gcc
CFLAGS = -g -Wall -std=c99 -fsanitize=address,undefined -pthread

all: nimd rawc test_suite

nimd: nimd.o network.o
	$(CC) $(CFLAGS) -o $@ $^

rawc: rawc.o pbuf.o network.o
	$(CC) $(CFLAGS) -o $@ $^

test_suite: test_suite.c network.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f nimd rawc test_suite *.o