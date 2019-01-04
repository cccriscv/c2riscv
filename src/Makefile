CC := gcc
CFLAGS = -O0 -std=gnu99 -Wall -Werror
TARGET = ../c0c

all: $(TARGET)

$(TARGET): c0c/c0c.c c0c/compiler.c c0c/lexer.c c0c/scan.c util/map.c util/strTable.c util/util.c vm/vm.c x86/x86.c
	$(CC) $(CFLAGS) $^ -o $(TARGET)

clean:
	rm *.o *.exe
