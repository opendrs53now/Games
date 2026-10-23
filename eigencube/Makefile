CC=gcc
GUILE_CFLAGS:=$(shell pkg-config --cflags guile-3.0)
GUILE_LIBS:=$(shell pkg-config --libs guile-3.0)
RAY_CFLAGS:=$(shell pkg-config --cflags raylib)
RAY_LIBS:=$(shell pkg-config --libs raylib)
CFLAGS=-Wall -Wextra -O2 $(GUILE_CFLAGS) $(RAY_CFLAGS)
LDFLAGS=$(RAY_LIBS) $(GUILE_LIBS) -lGL -lm -lpthread -ldl -lrt
BIN=eigencube
SRC=main.c
all: $(BIN)
$(BIN): $(SRC) scheme/live.scm
	$(CC) $(CFLAGS) -o $(BIN) $(SRC) $(LDFLAGS)
clean:; rm -f $(BIN)
