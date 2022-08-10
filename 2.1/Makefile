CC=g++
all: main

derivative: derivative.cpp
	$(CC) -D_debug derivative.cpp -o bin/derivative
	./bin/derivative

main: obj/main.o
	$(CC) -o bin/main obj/main.o
	./bin/main

obj/main.o: main.cpp
	$(CC) -c main.cpp -o obj/main.o

