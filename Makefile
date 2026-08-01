CXX = g++

CXXFLAGS = -Wall -Wextra -std=c++17 -Iinclude
LDFLAGS = -lncurses

TARGET = build/app

SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:src/%.cpp=build/%.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	@mkdir -p build
	$(CXX) $(OBJ) $(LDFLAGS) -o $@

build/%.o: src/%.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf build

run: $(TARGET)
	./$(TARGET)
