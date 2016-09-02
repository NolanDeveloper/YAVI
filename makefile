SOURSES = $(wildcard src/*)
OBJECTS = $(patsubst src/%.cpp, build/%.o, $(SOURSES))

CXX = clang++
CXXFLAGS = -std=c++14 -g -I./include -O2
LDLIBS = 

.PHONY: all
all: ./build/a.out
	@echo

.PHONY: clean
clean: 
	rm -rf ./build/*

./build/a.out: $(OBJECTS)
	$(CXX) $^ -o $@ $(LDLIBS) 

build/%.o: src/%.cpp
	@mkdir -p ./build
	$(CXX) -c $< -o $@ $(CXXFLAGS) $(LDLIBS)
	@{ echo -n ./build/; $(CXX) $< $(CXXFLAGS) -MM; } > build/$*.d 

-include $(wildcard build/*.d)
