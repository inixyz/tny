CXX := g++
CXXFLAGS := -Wall -Werror

SRC := src/*.cpp
TARGET := tnylsp

all:
	$(CXX) $(SRC) $(CXXFLAGS) -o $(TARGET)

.PHONY: clean
clean:
	rm $(TARGET)