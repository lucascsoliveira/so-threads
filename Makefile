CC=gcc
CFLAGS=-Wall
CLIBS=-lm -lpthread
EXEC=so-thread

.PHONY: all clean

all: $(EXEC)

clean:
	rm -f $(EXEC)
	rm -f *~

$(EXEC): Trabalho.c
	$(CC) -o $@ $< $(CFLAGS) $(CLIBS)
