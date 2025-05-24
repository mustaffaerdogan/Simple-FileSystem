CC=gcc
CFLAGS=-Wall -Wextra

all: simplefs

simplefs: main.o fs.o
	$(CC) $(CFLAGS) -o simplefs main.o fs.o

main.o: main.c fs.h
	$(CC) $(CFLAGS) -c main.c

fs.o: fs.c fs.h
	$(CC) $(CFLAGS) -c fs.c

clean:
	rm -f *.o simplefs
