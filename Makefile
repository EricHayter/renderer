CC = g++ -std=c++17
OUT_FILE = main

all: output

clean:
	rm *.o
	rm $(OUT_FILE)

debug: CC += -DEBUG -g3
debug: output

output: main.o model.o renderer.o vector3d.o
	$(CC) main.o model.o renderer.o vector3d.o -o $(OUT_FILE) -lSDL2

main.o: main.cpp
	$(CC) -c main.cpp

model.o: model.cpp
	$(CC) -c model.cpp

renderer.o: renderer.cpp
	$(CC) -c renderer.cpp

vector3d.o: vector3d.cpp
	$(CC) -c vector3d.cpp
