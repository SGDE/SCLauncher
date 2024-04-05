CC = gcc

CFLAGS = -lncurses

# File names

SRC = sclauncher.c

OBJ = $(SRC:.c=.o)

EXE = sclauncher

# Build executable files

all: $(EXE)

main: sclauncher.o

	$(CC) $(CFLAGS) -o $(EXE)

# Debug step

debug:

	$(CC) $(CFLAGS) -g $(SRC) -o debug

# Test step

test:

	./sclauncher

# Clean object files and executables

clean:

	rm -f $(OBJ) $(EXE) debug
