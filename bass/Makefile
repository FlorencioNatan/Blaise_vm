PROGRAM = bass
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -ggdb -std=c11 -g
C_FILES := $(wildcard src/*.c)
OBJS := $(patsubst %.c, %.o, $(C_FILES))
MISC_OBJS := $(wildcard ../misc/*/*.o)
TESTE_FILE = teste.bass
OUTPUT_TESTE_FILE = teste.bbvm
BVM_DIRECTORY = ../bvm/
DASS_DIRECTORY = ../dass/


all: $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(MISC_OBJS) -o build/$(PROGRAM)

run: build/$(PROGRAM)
	./build/$(PROGRAM) $(TESTE_FILE)
	cp $(OUTPUT_TESTE_FILE) $(BVM_DIRECTORY)
	cp $(OUTPUT_TESTE_FILE) $(DASS_DIRECTORY)

run_debug: build/$(PROGRAM)
	./build/$(PROGRAM) -d $(TESTE_FILE)
	cp $(OUTPUT_TESTE_FILE) $(BVM_DIRECTORY)
	cp $(OUTPUT_TESTE_FILE) $(DASS_DIRECTORY)

clean:
	rm $(OBJS)
	rm build/$(PROGRAM)
