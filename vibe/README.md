# Directed Acyclic Graph (DAG) Implementation in C++

This project provides a comprehensive implementation of a Directed Acyclic Graph (DAG) data structure in C++ with full template support.

## Features

- **Template-based design**: Works with any comparable data type (int, string, char, etc.)
- **Cycle detection**: Automatically prevents cycles when adding edges
- **Topological sorting**: Get a valid ordering of nodes
- **Node and edge management**: Add, remove, and query nodes and edges
- **Graph analysis**: Get in-degree, out-degree, neighbors, and predecessors
- **Comprehensive testing**: Full test suite demonstrating all functionality

## Files

- `dag.h` - Header file with class definition and method declarations
- `dag.cpp` - Implementation of all DAG methods
- `test_dag.cpp` - Comprehensive test suite
- `Makefile` - Build configuration for easy compilation
- `README.md` - This documentation file

## Building and Running

### Prerequisites
- C++17 compatible compiler (g++, clang++, etc.)
- Make utility

### Build Commands

```bash
# Build the test executable
make

# Build and run tests
make test

# Build with debug symbols
make debug

# Build optimized release version
make release

# Clean build artifacts
make clean

# Show help
make help
```

### Running Tests

```bash
# Build and run tests
make test

# Or run directly
./test_dag
```

## Usage Example

```cpp
#include "dag.h"
#include <iostream>

int main() {
    DAG<std::string> dag;
    
    // Add nodes and edges
    dag.addEdge("A", "B");
    dag.addEdge("B", "C");
    dag.addEdge("A", "C");
    
    // Check for cycles
    if (!dag.hasCycle()) {
        std::cout << "Graph is acyclic" << std::endl;
    }
    
    // Get topological sort
    auto sorted = dag.topologicalSort();
    for (const auto& node : sorted) {
        std::cout << node << " ";
    }
    std::cout << std::endl;
    
    return 0;
}
```

## API Reference

### Core Methods

- `addNode(const T& node)` - Add a node to the DAG
- `removeNode(const T& node)` - Remove a node and all its edges
- `addEdge(const T& source, const T& destination)` - Add an edge (throws if cycle would be created)
- `removeEdge(const T& source, const T& destination)` - Remove an edge
- `hasNode(const T& node)` - Check if a node exists
- `hasEdge(const T& source, const T& destination)` - Check if an edge exists

### Analysis Methods

- `hasCycle()` - Check if the graph contains any cycles
- `topologicalSort()` - Get a topological ordering of nodes
- `getNeighbors(const T& node)` - Get all outgoing neighbors of a node
- `getPredecessors(const T& node)` - Get all incoming neighbors of a node
- `getInDegree(const T& node)` - Get the in-degree of a node
- `getOutDegree(const T& node)` - Get the out-degree of a node

### Utility Methods

- `size()` - Get the number of nodes
- `empty()` - Check if the DAG is empty
- `clear()` - Remove all nodes and edges
- `print()` - Print the DAG structure to stdout
- `getNodes()` - Get a vector of all nodes

## Test Coverage

The test suite covers:

1. **Basic Operations**: Adding/removing nodes and edges
2. **Cycle Detection**: Preventing and detecting cycles
3. **Topological Sorting**: Valid ordering of nodes
4. **Node Operations**: Getting neighbors, predecessors, degrees
5. **Removal Operations**: Removing nodes and edges
6. **Complex Scenarios**: Real-world examples like build systems
7. **Print Functionality**: Visual representation of the DAG

## Time Complexity

- Adding/removing nodes: O(1)
- Adding/removing edges: O(V) where V is the number of nodes (due to cycle detection)
- Cycle detection: O(V + E) where E is the number of edges
- Topological sort: O(V + E)
- Getting neighbors/predecessors: O(degree of node)

## Space Complexity

- Overall: O(V + E) where V is the number of nodes and E is the number of edges
- Uses adjacency list representation for efficient edge traversal

## Error Handling

- Throws `std::invalid_argument` when trying to add an edge that would create a cycle
- Throws `std::runtime_error` when trying to perform topological sort on a cyclic graph
- Gracefully handles operations on non-existent nodes/edges

## License

This implementation is provided as-is for educational and practical use.
