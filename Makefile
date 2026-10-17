CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -g

.PHONY: all test clean

all: bin/test_cpu

bin/test_cpu: emulator/cpu.c emulator/cpu.h tests/test_cpu.c
	$(CC) $(CFLAGS) emulator/cpu.c tests/test_cpu.c -o $@

test: bin/test_cpu
	./bin/test_cpu

clean:
	rm -f bin/test_cpu

