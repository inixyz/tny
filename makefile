CXX := g++
CXXFLAGS := -std=c++23 -Wall -Werror

SRC := src/*.cpp
INCLUDE := src
TARGET := tnyvec

.PHONY: all format clean

all: format
	$(CXX) src/cli.cpp $(CXXFLAGS) -o $(TARGET)_cli
	$(CXX) src/repl.cpp $(CXXFLAGS) -o $(TARGET)_repl

format:
	clang-format -i --style=LLVM $(SRC) $(INCLUDE)/*.h

clean:
	rm $(TARGET)
