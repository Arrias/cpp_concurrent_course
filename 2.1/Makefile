CC=g++
all: main

derivative: derivative.cpp
	mkdir -p bin
	$(CC) -D_debug derivative.cpp -o bin/derivative
	./bin/derivative

main: obj/main.o
	mkdir -p obj
	$(CC) -o bin/main obj/main.o
	./bin/main

obj/main.o: main.cpp
	mkdir -p obj
	$(CC) -c main.cpp -o obj/main.o

