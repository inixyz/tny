CXX := g++
CXXFLAGS := -std=c++23 -Wall -Werror

SRC := src/*.cpp
TARGET := tnyvec

all:
	$(CXX) $(SRC) $(CXXFLAGS) -o $(TARGET)

.PHONY: clean
clean:
	rm $(TARGET)