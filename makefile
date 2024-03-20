CC := gcc
CCFLAGS := -Wall -DNum=double -Os

SRC := src/*.c
TARGET := tnylsp

all:
	$(CC) $(SRC) $(CCFLAGS) -o $(TARGET)

.PHONY: clean
clean:
	rm $(TARGET)