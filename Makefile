# Compiler and Flags
CC = g++
CXXFLAGS = -std=c++17
LDFLAGS = -lSDL2

# Output file
OUT_FILE = ./main

# Common flags for release and debug
RELEASE_FLAGS = -O3
DEBUG_FLAGS = -DEBUG -g3

# Sources and object files
SOURCES = main.cpp model.cpp renderer.cpp vector.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# Default target
all: $(OUT_FILE)

# Clean target
clean:
	rm -f *.o $(OUT_FILE)

# Debug target
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: $(OUT_FILE)

# Output target
$(OUT_FILE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(OUT_FILE) $(LDFLAGS) $(RELEASE_FLAGS)

# Object file rules
%.o: %.cpp
	$(CC) $(CXXFLAGS) -c $< -o $@

# Implicit rule for release build with optimization
release: CXXFLAGS += $(RELEASE_FLAGS)
release: $(OUT_FILE)
