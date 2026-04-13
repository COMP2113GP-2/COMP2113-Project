# Ocean Voyage - COMP2113 / ENGG1340 Group Project
# Role 1: Main Program & Integration
# Makefile for compiling and building the project

CC = g++
CFLAGS = -std=c++11 -Wall -Wextra

# All source files
SOURCES = main.cpp game.cpp ui.cpp event.cpp port.cpp save.cpp pet.cpp ui_display.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = ocean_voyage

# Build the main executable
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

# Compile .cpp files to .o object files
.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

# Clean compiled files and saved data
clean:
	rm -f $(OBJECTS) $(EXECUTABLE) save.txt