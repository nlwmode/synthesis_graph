main: demo.o dag.o
	gcc -o main demo.o dag.o -lm