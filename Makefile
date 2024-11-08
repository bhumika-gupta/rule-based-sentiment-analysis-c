CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g

SOURCES = parser.c
OBJ = $(SOURCES:.c=.o)
EXECUTABLE = mySA

all: $(EXECUTABLE)
$(EXECUTABLE): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@ -lm

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXEC)