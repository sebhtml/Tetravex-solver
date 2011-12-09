all: solver

solver: solver.cpp
	g++ -O3 -Wall solver.cpp -o solver

debug: solver.cpp
	g++ -g -Wall solver.cpp -o debug

test: solver
	./solver 6x6.txt
