all: aufg4

run: generateFiles test

generateFiles:
	-sh generateFiles.sh

test: aufg4
	sh test.sh

aufg4: aufg4.o
	gcc -std=c99 -lpthread -o $@ $<

aufg4.o: aufg4.c clean
	gcc -std=c99 -lpthread -c $<

clean:
	-rm -r aufg4 aufg4.o
