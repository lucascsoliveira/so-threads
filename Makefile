CC=gcc
CFLAGS=-Wall
CLIBS=-lm -lpthread
EXEC=thread

.PHONY: all clean

all: $(EXEC)

clean:
	rm -f $(EXEC)
	rm -f *~

$(EXEC): thread.c
	$(CC) -o $@ $< $(CFLAGS) $(CLIBS)
