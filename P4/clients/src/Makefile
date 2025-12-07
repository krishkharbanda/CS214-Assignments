CC = gcc
CFLAGS = -g -Wall -std=c99 -fsanitize=address,undefined -pthread

all: nimd rawc test_suite testc

nimd: nimd.o network.o
	$(CC) $(CFLAGS) -o $@ $^

rawc: rawc.o pbuf.o network.o
	$(CC) $(CFLAGS) -o $@ $^

test_suite: test_suite.c network.o
	$(CC) $(CFLAGS) -o $@ $^

test: test_suite
	@echo "-------------------------------------------------------"
	@echo "WARNING: Ensure './nimd 5001' is running in another terminal!"
	@echo "Starting Test Suite on port 5001..."
	@echo "-------------------------------------------------------"
	@sleep 1
	
	@echo "\n[1/4] Running Happy Path (Standard Game)..."
	./test_suite happy 5001
	@sleep 1
	
	@echo "\n[2/4] Running Concurrency Test..."
	./test_suite concurrent 5001
	@sleep 1
	
	@echo "\n[3/4] Running Error Handling Tests..."
	./test_suite errors 5001
	@sleep 1
	
	@echo "\n[4/4] Running Extra Credit Test (Impatience)..."
	./test_suite extra 5001
	
	@echo "\n-------------------------------------------------------"
	@echo "All tests completed."

clean:
	rm -f nimd rawc test_suite testc *.o