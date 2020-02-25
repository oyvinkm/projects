CC=gcc
CFLAGS=-g -Wall -Wextra -pedantic -std=gnu99 -pthread
EXAMPLES=fibs fauxgrep fauxgrep-mt fhistogram fhistogram-mt

.PHONY: all test clean ../src.zip

all: $(TESTS) $(EXAMPLES)

job_queue.o: job_queue.c job_queue.h
	$(CC) -c job_queue.c $(CFLAGS)

%: %.c job_queue.o
	$(CC) -o $@ $^ $(CFLAGS)

test:
	./test.sh

clean:
	rm -rf $(TESTS) $(EXAMPLES) *.o core
	rm -f *.expected *.actual

../src.zip:
	make clean
	cd .. && zip src.zip -r src
