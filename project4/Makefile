# Makefile for project 4 part 1

# Compiles program
all: pr4.c
	gcc -Wall -g -lcrypto -o pr4 pr4.c
	gcc -Wall -g -lcrypto -lpthread -o pr4_p pr4_p.c

# Removes executable
clean:
	rm pr4
	rm pr4_p

# Runs executable
test: 
	time -p ./pr4_p transactions2.csv 8

# Runs valgrind on executable
test_valgrind:
	valgrind --leak-check=full pr4_p transactions2_short.csv