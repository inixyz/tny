CC := gcc
CCFLAGS := -Wall -DNrType=double

SRC := src/*.c
TARGET := tny

all:
	$(CC) $(SRC) $(CCFLAGS) -o $(TARGET)

.PHONY: clean
clean:
	rm $(TARGET)