# gcc -o nemesis $(/home/kyle/sdl2/bin/sdl2-config --libs && pkg-config gl --libs) $(/home/kyle/sdl2/bin/sdl2-config --cflags) src/main.c
CC = gcc
SDL2_PATH = ~/sdl2/
OPT = 0
DBG = 3
MYCFLAGS =
MYLDFLAGS =
CFLAGS = $(shell $(SDL2_PATH)/bin/sdl2-config --cflags) -O$(OPT) -g$(DBG) $(MYCFLAGS)
LDFLAGS = $(shell $(SDL2_PATH)/bin/sdl2-config --libs) -lSDL2_image $(MYLDFLAGS)
OBJDIR = obj
OUT = nemesis

.PHONY: clean
default: $(OUT)

SRC = $(wildcard src/*.c)
HDR = $(wildcard src/*.h)
OBJ = $(patsubst src/%.c,$(OBJDIR)/%.o,$(SRC))

$(OBJDIR)/%.o: src/%.c $(HEADERS)
	@mkdir -p $(OBJDIR)
	$(CC) -o $@ $(CFLAGS) -c $<

$(OUT): $(OBJ)
	$(CC) -o $(OUT) $(OBJ) $(LDFLAGS)

clean:
	@rm -rf $(OUT) $(OBJDIR)
