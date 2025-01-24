TAG=parser
DOCKER=docker
# Object files
OBJECTS = main.o src/Regexp.o parser.o

# Executable file
EXECUTABLE = regexp

# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -std=c++11 -g -O0 # You can adjust the standard as needed

# Rule to build the executable
$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Rule to compile object files
%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Phony target to clean up object files and executable
clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

r:
	./$(EXECUTABLE) testfile.txt

d:
	gdbgui --args ../$(EXECUTABLE) testfile.txt

dr:
	${DOCKER} run --rm -h ${TAG} -p 5000:5000 --name ${TAG} -it  -v ~/projects:/projects \
		--workdir /projects/arduino/fingerkey/parser/Regexp ${TAG}:latest  /bin/bash


attach:
	${DOCKER} attach ${TAG}

stop:
	${DOCKER} stop ${TAG}
	${DOCKER} rm ${TAG}
#	${DOCKER} stop `${DOCKER} ps -a | grep ${TAG}:latest | grep -v Exited | cut -d' ' -f1`

db:
	cp ~/.tmux.conf tmux.conf
	${DOCKER} build -f ${TAG}.Dockerfile . --tag=${TAG}
	rm tmux.conf
