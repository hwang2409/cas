#include "dag.h"
#include <iostream>
#include <string>
#include <cassert>

void testBasicOperations() {
    std::cout << "=== Testing Basic Operations ===" << std::endl;
    
    dag<std::string> graph;
    
    // Test adding nodes
    graph.add_node("A");
    graph.add_node("B");
    graph.add_node("C");
    graph.add_node("D");
    
    assert(graph.size() == 4);
    assert(graph.has_node("A"));
    assert(graph.has_node("B"));
    assert(!graph.has_node("E"));
    
    // Test adding edges
    graph.add_edge("A", "B");
    graph.add_edge("B", "C");
    graph.add_edge("A", "D");
    graph.add_edge("C", "D");
    
    assert(graph.has_edge("A", "B"));
    assert(graph.has_edge("B", "C"));
    assert(!graph.has_edge("B", "A"));
    
    std::cout << "Basic operations test passed!" << std::endl;
}

void testCycleDetection() {
    std::cout << "\n=== Testing Cycle Detection ===" << std::endl;
    
    dag<int> graph;
    
    // Create a DAG without cycles
    graph.add_edge(1, 2);
    graph.add_edge(2, 3);
    graph.add_edge(1, 3);
    graph.add_edge(3, 4);
    
    assert(!graph.has_cycle());
    std::cout << "No cycle test passed!" << std::endl;
    
    // Try to add an edge that would create a cycle
    try {
        graph.add_edge(4, 1); // This should create a cycle
        assert(false); // Should not reach here
    } catch (const std::invalid_argument& e) {
        std::cout << "Cycle prevention test passed! Exception: " << e.what() << std::endl;
    }
}

void testTopologicalSort() {
    std::cout << "\n=== Testing Topological Sort ===" << std::endl;
    
    dag<std::string> graph;
    
    // Create a DAG representing course prerequisites
    graph.add_edge("Math101", "Math201");
    graph.add_edge("Math101", "Physics101");
    graph.add_edge("Physics101", "Physics201");
    graph.add_edge("Math201", "Math301");
    graph.add_edge("Physics201", "Physics301");
    graph.add_edge("Math301", "Physics301");
    
    std::vector<std::string> sorted = graph.topological_sort();
    
    std::cout << "Topological sort result: ";
    for (size_t i = 0; i < sorted.size(); ++i) {
        std::cout << sorted[i];
        if (i < sorted.size() - 1) std::cout << " -> ";
    }
    std::cout << std::endl;
    
    // Verify that prerequisites come before courses
    auto findIndex = [&](const std::string& course) {
        for (size_t i = 0; i < sorted.size(); ++i) {
            if (sorted[i] == course) return i;
        }
        return sorted.size();
    };
    
    assert(findIndex("Math101") < findIndex("Math201"));
    assert(findIndex("Math101") < findIndex("Physics101"));
    assert(findIndex("Math201") < findIndex("Math301"));
    assert(findIndex("Physics101") < findIndex("Physics201"));
    assert(findIndex("Physics201") < findIndex("Physics301"));
    assert(findIndex("Math301") < findIndex("Physics301"));
    
    std::cout << "Topological sort test passed!" << std::endl;
}

void testNodeOperations() {
    std::cout << "\n=== Testing Node Operations ===" << std::endl;
    
    dag<char> graph;
    
    // Add nodes and edges
    graph.add_edge('A', 'B');
    graph.add_edge('B', 'C');
    graph.add_edge('A', 'C');
    graph.add_edge('C', 'D');
    
    // Test getting neighbors
    auto neighbors = graph.get_neighbors('A');
    assert(neighbors.size() == 2);
    assert(std::find(neighbors.begin(), neighbors.end(), 'B') != neighbors.end());
    assert(std::find(neighbors.begin(), neighbors.end(), 'C') != neighbors.end());
    
    // Test getting predecessors
    auto predecessors = graph.get_predecessors('C');
    assert(predecessors.size() == 2);
    assert(std::find(predecessors.begin(), predecessors.end(), 'A') != predecessors.end());
    assert(std::find(predecessors.begin(), predecessors.end(), 'B') != predecessors.end());
    
    // Test degrees
    assert(graph.get_out_degree('A') == 2);
    assert(graph.get_in_degree('C') == 2);
    assert(graph.get_out_degree('D') == 0);
    assert(graph.get_in_degree('A') == 0);
    
    std::cout << "Node operations test passed!" << std::endl;
}

void testRemoval() {
    std::cout << "\n=== Testing Node and Edge Removal ===" << std::endl;
    
    dag<int> graph;
    
    // Create a DAG
    graph.add_edge(1, 2);
    graph.add_edge(2, 3);
    graph.add_edge(1, 3);
    graph.add_edge(3, 4);
    
    assert(graph.size() == 4);
    assert(graph.has_edge(1, 2));
    
    // Remove an edge
    graph.remove_edge(1, 2);
    assert(!graph.has_edge(1, 2));
    assert(graph.size() == 4); // Node count should remain the same
    
    // Remove a node
    graph.remove_node(2);
    assert(!graph.has_node(2));
    assert(!graph.has_edge(1, 2));
    assert(!graph.has_edge(2, 3));
    assert(graph.size() == 3);
    
    std::cout << "Removal test passed!" << std::endl;
}

void testComplexDAG() {
    std::cout << "\n=== Testing Complex DAG ===" << std::endl;
    
    dag<std::string> graph;
    
    // Create a complex DAG representing a build system
    graph.add_edge("source1.cpp", "object1.o");
    graph.add_edge("source2.cpp", "object2.o");
    graph.add_edge("source3.cpp", "object3.o");
    graph.add_edge("object1.o", "library.a");
    graph.add_edge("object2.o", "library.a");
    graph.add_edge("object3.o", "library.a");
    graph.add_edge("library.a", "executable");
    graph.add_edge("header1.h", "source1.cpp");
    graph.add_edge("header1.h", "source2.cpp");
    graph.add_edge("header2.h", "source2.cpp");
    graph.add_edge("header2.h", "source3.cpp");
    
    assert(!graph.has_cycle());
    
    auto sorted = graph.topological_sort();
    std::cout << "Build order: ";
    for (size_t i = 0; i < sorted.size(); ++i) {
        std::cout << sorted[i];
        if (i < sorted.size() - 1) std::cout << " -> ";
    }
    std::cout << std::endl;
    
    // Verify that headers come before source files
    auto findIndex = [&](const std::string& item) {
        for (size_t i = 0; i < sorted.size(); ++i) {
            if (sorted[i] == item) return i;
        }
        return sorted.size();
    };
    
    assert(findIndex("header1.h") < findIndex("source1.cpp"));
    assert(findIndex("header1.h") < findIndex("source2.cpp"));
    assert(findIndex("source1.cpp") < findIndex("object1.o"));
    assert(findIndex("object1.o") < findIndex("library.a"));
    assert(findIndex("library.a") < findIndex("executable"));
    
    std::cout << "Complex DAG test passed!" << std::endl;
}

void testPrintFunction() {
    std::cout << "\n=== Testing Print Function ===" << std::endl;
    
    dag<std::string> graph;
    graph.add_edge("A", "B");
    graph.add_edge("A", "C");
    graph.add_edge("B", "D");
    graph.add_edge("C", "D");
    
    std::cout << "DAG structure:" << std::endl;
    graph.print();
    
    std::cout << "Print test completed!" << std::endl;
}

int main() {
    std::cout << "Starting DAG Tests..." << std::endl;
    
    try {
        testBasicOperations();
        testCycleDetection();
        testTopologicalSort();
        testNodeOperations();
        testRemoval();
        testComplexDAG();
        testPrintFunction();
        
        std::cout << "\n=== All Tests Passed! ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
