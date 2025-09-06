PROGRAM = pascal
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -ggdb -std=c11 -g
CLIBS = -lgvc -lcgraph -lcdt
C_FILES := $(wildcard src/flex_bison/*.c src/*.c)
OBJS := $(patsubst %.c, %.o, $(C_FILES))
LEXER := src/flex_bison/lexer.c
PARSER := src/flex_bison/parser.c

$(PROGRAM): $(PARSER) $(LEXER) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o build/$(PROGRAM) $(CLIBS)

$(PARSER): parser.y
	bison -d -v -o src/flex_bison/parser.c parser.y

$(LEXER): lexer.l
	flex --header-file=src/flex_bison/lexer.h -o src/flex_bison/lexer.c lexer.l
	sed -i '24i int fileno(FILE *stream);' src/flex_bison/lexer.c 

run:
	./build/pascal teste.pas

clean:
	rm $(LEXER) src/flex_bison/lexer.h
	rm $(PARSER) src/flex_bison/parser.h
	rm $(OBJS)
	rm build/$(PROGRAM)


