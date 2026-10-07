# (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

# compiler
CXX = g++
# CXX = clang++

# compiler flags
CXXFLAGS = -pedantic -Wall -Wextra -Wconversion -std=c++23

# preprocessor flags: where #include looks
CPPFLAGS = -I../../utils

# linker flags
# LDFLAGS = -lm

# marks a target as a command name
.PHONY: all clean

# says what to build and because it's the first target it's the default goal.
all: a_helloworld.out b_helloworld_revisited.out c_helloworld_codebook.out d_uniform_initialization.out

# explicit rule
a_helloworld.out: a_helloworld.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) a_helloworld.cpp $(LDFLAGS) -o a_helloworld.out

# b_helloworld_revisited.out: b_helloworld_revisited.cpp
#	$(CXX) $(CPPFLAGS) $(CXXFLAGS) b_helloworld_revisited.cpp $(LDFLAGS) -o b_helloworld_revisited.out

# extra prerequisite only - no recipe, so the pattern rule below builds it
c_helloworld_codebook.out: ../../utils/cbl/printing.hpp
d_uniform_initialization.out: ../../utils/cbl/printing.hpp

# one pattern rule instead of one explicit rule per file
# '%' matches the stem, '$<' is the first prerequisite (the .cpp),
# '$@' is the target (the .out)
%.out: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< $(LDFLAGS) -o $@

clean:
	rm -f *.out
