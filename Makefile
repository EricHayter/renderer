LDFLAGS = -lSDL2
CXXFLAGS = -std=c++23 -Wall -Wextra -Wpedantic -Weffc++
RELEASE_FLAGS = -O3 # -Werror
DEBUG_FLAGS = -DEBUG -g3

OUT_FILE = ./main
SOURCES = $(wildcard src/*.cpp)
HEADERS = $(wildcard src/*.h)
OBJECTS = $(SOURCES:.cpp=.o)

all: $(OUT_FILE)

$(OUT_FILE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(OUT_FILE) $(LDFLAGS) $(RELEASE_FLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(OUT_FILE)

format:
	clang-format -i --style=file $(SOURCES) $(HEADERS)

debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: $(OUT_FILE)

release: CXXFLAGS += $(RELEASE_FLAGS)
release: $(OUT_FILE)
