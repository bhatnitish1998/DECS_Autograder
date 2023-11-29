# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++17

# Source files
SOURCES = $(wildcard *.cpp)
HEADER_FILES = $(wildcard *.hpp)
DEPS = $(wildcard *.d)

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Output directory for the executable
OUTPUT_DIR = bin

EXECUTABLE = $(OUTPUT_DIR)/submit

all: $(OUTPUT_DIR) $(EXECUTABLE)

$(OUTPUT_DIR):
	mkdir -p $(OUTPUT_DIR)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJECTS)

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -MMD -MP $< -o $@

-include $(SOURCES:.cpp=.d)
.PHONY: all clean
clean:
	rm -f $(OBJECTS) $(EXECUTABLE) $(DEPS)