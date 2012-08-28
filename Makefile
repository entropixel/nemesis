CC = gcc
SDL2_PATH = ~/sdl2/
OPT = 0
DBG = 3
MYCFLAGS =
MYLDFLAGS =
GIT_VERSION = $(shell git rev-parse --short HEAD)-$(shell date +%y%m%d)
CFLAGS = -std=c99 -pedantic $(shell $(SDL2_PATH)/bin/sdl2-config --cflags) -O$(OPT) -g$(DBG) -DGIT_VERSION="\"$(GIT_VERSION)\"" $(MYCFLAGS)
LDFLAGS = $(shell $(SDL2_PATH)/bin/sdl2-config --libs) -lm $(MYLDFLAGS)
OBJDIR = obj
OUT = nemesis

.PHONY: clean
default: $(OUT)

SRC = $(wildcard src/*.c)
HDR = $(wildcard src/*.h)
OBJ = $(patsubst src/%.c,$(OBJDIR)/%.o,$(SRC))

$(OBJDIR)/%.o: src/%.c $(HDR)
	@mkdir -p $(OBJDIR)
	$(CC) -o $@ $(CFLAGS) -c $<

$(OUT): $(OBJ)
	$(CC) -o $(OUT) $(OBJ) $(LDFLAGS)

clean:
	@rm -rf $(OUT) $(OBJDIR)
