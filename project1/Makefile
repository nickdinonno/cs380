# Makefile for project 1

# Compiles program
all: pr1.c
	gcc -Wall -g -o pr1 pr1.c

# Removes executable
clean:
	rm pr1

# Runs executable
test: 
	./pr1 transactions.csv

# Runs valgrind on executable
test_valgrind:
	valgrind --leak-check=full pr1 transactions.csv