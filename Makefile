
GCC = gcc
FLAGS = -Wall -Wextra

# cs340 test command
# g++ -std=c++20 -Wall -Wextra -Wold-style-cast -Werror -pthread -o a09.exe a09.cpp -ltbb

NAME = fake6502.c mem.c
BIN = 6502

.SUFFIXES : .c .exe

all: clean build run

# compile all .java files in src/
build:
	$(GCC) $(FLAGS) $(NAME) -o $(BIN).o

run: 
	./$(BIN).o atlantis.bin

clean:
	rm -f -r *.o

fib: clean build
	$(GCC) $(FLAGS) fake6502.c fib.c -o fib.o
	./fib.o atlantis.bin fib.bin 


.PHONY: build run clean
