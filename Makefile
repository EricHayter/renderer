output: main.o model.o renderer.o
	g++ -std=c++17 main.o model.o renderer.o -o main -lSDL2

main.o: main.cpp
	g++ -c main.cpp

model.o: model.cpp
	g++ -c model.cpp

renderer.o: renderer.cpp
	g++ -c renderer.cpp
