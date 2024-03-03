CXX := g++
CXXFLAGS := -Wall -std=c++23

SRC := src/*.cpp
TARGET := tny

all:
	$(CXX) $(SRC) $(CXXFLAGS) -o $(TARGET)

tests:
	$(CXX) test/test_lex.cpp src/lex.cpp $(CXXFLAGS) -I src -o test_lex

.PHONY: clean
clean:
	rm $(TARGET) test_lex