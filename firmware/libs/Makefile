# This is just for testing the libraries.  They are build with
# firmware/Makefile when running in the watch.

EXECS= assembler pocsag jukebox hebrew beats phonebook

run: all
	./assembler
	./pocsag
	./jukebox
	./hebrew
	./beats
	./phonebook

clean:
	rm -rf *.o $(EXECS)
all: $(EXECS)


assembler: assembler.c assembler.h
	$(CC) -Werror -DSTANDALONE -o assembler $<

pocsag: pocsag.c pocsag.h
	$(CC) -Werror -DSTANDALONE -o pocsag $<

jukebox: jukebox.c jukebox.h
	$(CC) -DSTANDALONE -o jukebox $<

hebrew: hebrew.c hebrew.h
	$(CC) -Werror -DSTANDALONE -o hebrew $<

beats: beats.c beats.h
	$(CC) -Werror -DSTANDALONE -o beats $<

phonebook: phonebook.c phonebook.h
	$(CC) -Werror -DSTANDALONE -o phonebook $<
