# This is just for testing the libraries.  They are build with
# firmware/Makefile when running in the watch.

EXECS= assembler pocsag jukebox bruijn

run: all
	./assembler
	./pocsag
	./jukebox
	./bruijn

clean:
	rm -rf *.o $(EXECS)
all: $(EXECS)


assembler: assembler.c assembler.h
	$(CC) -DSTANDALONE -o assembler $<

pocsag: pocsag.c pocsag.h
	$(CC) -Werror -DSTANDALONE -o pocsag $<

jukebox: jukebox.c jukebox.h
	$(CC) -DSTANDALONE -o jukebox $<

bruijn: bruijn.c bruijn.h
	$(CC) -Wall -g -DSTANDALONE -o bruijn $<  -lm



