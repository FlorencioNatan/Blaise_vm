PROGRAM = bass
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -ggdb -std=c11
C_FILES := $(wildcard src/*.c)
OBJS := $(patsubst %.c, %.o, $(C_FILES))
MISC_OBJS := $(wildcard ../misc/*/*.o)
TESTE_FILE = teste.bass
OUTPUT_TESTE_FILE = teste.bbvm
BVM_DIRECTORY = ../bvm/


all: $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(MISC_OBJS) -o build/$(PROGRAM)

run: build/$(PROGRAM)
	./build/$(PROGRAM) $(TESTE_FILE)
	cp $(OUTPUT_TESTE_FILE) $(BVM_DIRECTORY)

run_debug: build/$(PROGRAM)
	./build/$(PROGRAM) -d $(TESTE_FILE)
	cp $(OUTPUT_TESTE_FILE) $(BVM_DIRECTORY)

clean:
	rm $(OBJS)
	rm build/$(PROGRAM)
