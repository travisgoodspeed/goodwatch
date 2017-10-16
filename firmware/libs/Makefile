# This is just for testing the libraries.  They are build with
# firmware/Makefile when running in the watch.

run: all
	./assembler

clean:
	rm -rf *.o
all: assembler


assembler: assembler.c assembler.h
	$(CC) -DSTANDALONE -o assembler $<
