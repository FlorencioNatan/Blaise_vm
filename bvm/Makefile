PROGRAM = bvm
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -ggdb -std=c11 -g
LDLIBS = -lm
LIBS = ./lib/raylib/libraylib.a
C_FILES := $(wildcard src/*.c)
OBJS := $(patsubst %.c, %.o, $(C_FILES))
MISC_OBJS := $(wildcard ../misc/*/*.o)
TESTE_FILE = teste.bbvm


all: $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(MISC_OBJS) -o build/$(PROGRAM) $(LIBS) $(LDLIBS)

run: build/$(PROGRAM)
	./build/$(PROGRAM) $(TESTE_FILE)
