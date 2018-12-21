CC := riscv64-unknown-elf-gcc
CFLAGS = -std=c99 -O0
TARGET = run.o

all: $(TARGET)

run:
	spike pk $(TARGET)

$(TARGET): sum.c main.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f *.o