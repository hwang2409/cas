CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
SOURCES = rat.cpp utils.cpp main.cpp
HEADERS = rat.hpp utils.hpp dag.hpp dag.cpp edag.hpp edag.cpp
OUTPUT = main

default:
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(OUTPUT) && ./$(OUTPUT)
