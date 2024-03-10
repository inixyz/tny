CC := gcc
CCFLAGS := -Wall

SRC := src/*.c
TARGET := tny

all:
	$(CC) $(SRC) $(CCFLAGS) -o $(TARGET)

.PHONY: clean
clean:
	rm $(TARGET)