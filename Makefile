CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
SOURCES = main.cpp
HEADERS = dag.hpp dag.cpp edag.hpp edag.cpp
OUTPUT = main

default:
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(OUTPUT) && ./$(OUTPUT)
