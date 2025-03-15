main: main.o tetris.o
	gcc -o main main.o tetris.o

main.o: main.c
	gcc -c main.c

tetris.o: tetris.c tetris.h
	gcc -c tetris.c

clean:
	rm -f *.o tetris

run: main
	./main