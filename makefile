CC := gcc
CCFLAGS := -Wall -DNum=double

SRC := src/*.c
TARGET := tnylsp

all:
	$(CC) $(SRC) $(CCFLAGS) -o $(TARGET)

.PHONY: clean
clean:
	rm $(TARGET)