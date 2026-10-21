CC=gcc
GUILE_INC:=$(shell find /gnu/store -type d -path "*guile*include/guile/3.0" 2>/dev/null | head -n1)
ifeq ($(GUILE_INC),)
  GUILE_CFLAGS:=$(shell pkg-config --cflags guile-3.0 2>/dev/null || pkg-config --cflags guile-2.2 2>/dev/null)
  GUILE_LIBS:=$(shell pkg-config --libs guile-3.0 2>/dev/null || pkg-config --libs guile-2.2 2>/dev/null || echo "-lguile-3.0")
  RAYLIB_LIBS:=$(shell pkg-config --libs raylib 2>/dev/null || echo "-lraylib")
else
  RAYLIB_LIB:=$(shell find /gnu/store -type d -name lib -path "*raylib*" 2>/dev/null | head -n1)
  GUILE_LIB:=$(shell find /gnu/store -type d -name lib -path "*guile-3*" 2>/dev/null | head -n1)
  GUILE_CFLAGS:=-I$(GUILE_INC)/.. -I$(GUILE_INC)
  GUILE_LIBS:=-L$(GUILE_LIB) -lguile-3.0
  RAYLIB_LIBS:=-L$(RAYLIB_LIB) -lraylib
endif
CFLAGS=-Wall -Wextra -O2 $(GUILE_CFLAGS) $(shell pkg-config --cflags raylib 2>/dev/null)
LDFLAGS=$(RAYLIB_LIBS) $(GUILE_LIBS) -lm -lGL -lpthread -ldl
eigencube: src/main.c
	$(CC) $(CFLAGS) -o eigencube src/main.c $(LDFLAGS)
clean:
	rm -f eigencube
