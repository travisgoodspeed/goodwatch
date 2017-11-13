# This is just for testing the libraries.  They are build with
# firmware/Makefile when running in the watch.

EXECS= assembler

run: all
	./assembler

clean:
	rm -rf *.o $(EXECS)
all: $(EXECS)


assembler: assembler.c assembler.h
	$(CC) -DSTANDALONE -o assembler $<
