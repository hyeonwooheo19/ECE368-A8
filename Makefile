CC = gcc
CFLAGS = -Wall -Wextra -std=c11
EXE = a8
SRCS = a8.c
HEADERS = a8.h

all: $(EXE)

$(EXE): $(SRCS) $(HEADERS)
	$(CC) $(CFLAGS) -o $(EXE) $(SRCS)

clean:
	rm -f $(EXE)

.PHONY: all clean