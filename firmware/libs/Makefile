# This is just for testing the libraries.  They are build with
# firmware/Makefile when running in the watch.

EXECS= assembler pocsag jukebox

run: all
	./assembler
	./pocsag
	./jukebox

clean:
	rm -rf *.o $(EXECS)
all: $(EXECS)


assembler: assembler.c assembler.h
	$(CC) -DSTANDALONE -o assembler $<

pocsag: pocsag.c pocsag.h
	$(CC) -Werror -DSTANDALONE -o pocsag $<

jukebox: jukebox.c jukebox.h
	$(CC) -DSTANDALONE -o jukebox $<