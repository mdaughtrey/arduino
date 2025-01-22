# Object files
OBJECTS = main.o src/Regexp.o

# Executable file
EXECUTABLE = regexp

# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -std=c++11 # You can adjust the standard as needed

# Rule to build the executable
$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Rule to compile object files
%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Phony target to clean up object files and executable
clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
