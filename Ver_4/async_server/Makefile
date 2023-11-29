# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++17

# Include directories
INCLUDES = -I/usr/include
L_INCLUDES = -Iheaders

# Libraries
LIBS = -lpqxx -lpq

# Source files
SOURCES = $(wildcard src/*.cpp)
HEADER_FILES = $(wildcard headers/*.hpp)
DEPS = $(wildcard src/*.d)

S_SOURCES = $(filter-out src/Response.cpp src/ResponseServer.cpp ,$(SOURCES))
R_SOURCES = $(filter-out src/Submission.cpp src/SubmissionServer.cpp,$(SOURCES))



# Object files
OBJECTS = $(SOURCES:.cpp=.o)
S_OBJECTS = $(filter-out src/Response.o src/ResponseServer.o ,$(OBJECTS))
R_OBJECTS = $(filter-out src/Submission.o src/SubmissionServer.o ,$(OBJECTS))

# Output directory for the executable
OUTPUT_DIR = bin

# Executable name
SUBMISSION_EXECUTABLE = $(OUTPUT_DIR)/submission_server
RESPONSE_EXECUTABLE = $(OUTPUT_DIR)/response_server

all: submission response

submission: $(OUTPUT_DIR) $(SUBMISSION_EXECUTABLE)
response: $(OUTPUT_DIR) $(RESPONSE_EXECUTABLE)

$(OUTPUT_DIR):
	mkdir -p $(OUTPUT_DIR)

$(SUBMISSION_EXECUTABLE): $(S_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $(S_OBJECTS) $(LIBS)

$(RESPONSE_EXECUTABLE): $(R_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $(R_OBJECTS) $(LIBS)

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(INCLUDES) $(L_INCLUDES) -MMD -MP $< -o $@ $(LIBS)

-include $(SOURCES:.cpp=.d)
.PHONY: all clean
clean:
	rm -f $(OBJECTS) $(EXECUTABLE) $(DEPS)