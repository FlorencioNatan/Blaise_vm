CC = gcc
CFLAGS = -c -Wall -Wextra -pedantic -ggdb -std=c11 -g
LDLIBS = -lm
C_FILES := $(wildcard */*.c)
OBJS := $(patsubst %.c, %.o, $(C_FILES))


all: $(OBJS)
	$(CC) $(CFLAGS)  $(OBJS) $(LIBS) $(LDLIBS)

run: build/$(PROGRAM)
	./build/$(PROGRAM) $(TESTE_FILE)

clean:
	rm $(OBJS)
	rm build/$(PROGRAM)
	
