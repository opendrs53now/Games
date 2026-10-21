CC=gcc
CFLAGS=-Wall -Wextra -O2 $(shell pkg-config --cflags raylib)
LDFLAGS=$(shell pkg-config --libs raylib) -lm

pong: main.c paddle.c ball.c score.c paddle.h ball.h score.h
	$(CC) $(CFLAGS) main.c paddle.c ball.c score.c -o pong $(LDFLAGS)

clean:
	rm -f pong
