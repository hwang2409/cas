#ifndef DAG_H
#define DAG_H

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <stack>
#include <algorithm>
#include <stdexcept>
#include <iostream>

template<typename T>
class dag {
private:
    std::unordered_map<T, std::unordered_set<T>> adjacency_list;
    std::unordered_set<T> nodes;
    
    // Helper function for DFS-based cycle detection
    bool has_cycle_dfs(const T& node, std::unordered_set<T>& visited, 
                       std::unordered_set<T>& rec_stack) const;
    
    // Helper function for topological sort DFS
    void topological_sort_dfs(const T& node, std::unordered_set<T>& visited, 
                              std::stack<T>& result) const;

public:
    // Constructor
    dag() = default;
    
    // Destructor
    ~dag() = default;
    
    // Add a node to the DAG
    void add_node(const T& node);
    
    // Remove a node and all its edges
    void remove_node(const T& node);
    
    // Add an edge from source to destination
    // Throws exception if adding edge would create a cycle
    void add_edge(const T& source, const T& destination);
    
    // Remove an edge from source to destination
    void remove_edge(const T& source, const T& destination);
    
    // Check if an edge exists
    bool has_edge(const T& source, const T& destination) const;
    
    // Check if a node exists
    bool has_node(const T& node) const;
    
    // Get all nodes
    std::vector<T> get_nodes() const;
    
    // Get neighbors of a node
    std::vector<T> get_neighbors(const T& node) const;
    
    // Get predecessors of a node
    std::vector<T> get_predecessors(const T& node) const;
    
    // Check if the graph has any cycles
    bool has_cycle() const;
    
    // Get topological sort of the DAG
    std::vector<T> topological_sort() const;
    
    // Get the number of nodes
    size_t size() const;
    
    // Check if the DAG is empty
    bool empty() const;
    
    // Clear all nodes and edges
    void clear();
    
    // Print the DAG structure
    void print() const;
    
    // Get the in-degree of a node
    int get_in_degree(const T& node) const;
    
    // Get the out-degree of a node
    int get_out_degree(const T& node) const;
};

#endif // DAG_H
