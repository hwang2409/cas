#include "dag.h"

template<typename T>
void dag<T>::add_node(const T& node) {
    nodes.insert(node);
    if (adjacency_list.find(node) == adjacency_list.end()) {
        adjacency_list[node] = std::unordered_set<T>();
    }
}

template<typename T>
void dag<T>::remove_node(const T& node) {
    if (nodes.find(node) == nodes.end()) {
        return; // Node doesn't exist
    }
    
    // Remove all edges pointing to this node
    for (auto& pair : adjacency_list) {
        pair.second.erase(node);
    }
    
    // Remove the node and its outgoing edges
    nodes.erase(node);
    adjacency_list.erase(node);
}

template<typename T>
void dag<T>::add_edge(const T& source, const T& destination) {
    // Add nodes if they don't exist
    add_node(source);
    add_node(destination);
    
    // Add the edge
    adjacency_list[source].insert(destination);
    
    // Check if adding this edge creates a cycle
    if (has_cycle()) {
        // Remove the edge and throw exception
        adjacency_list[source].erase(destination);
        throw std::invalid_argument("Adding edge would create a cycle in the DAG");
    }
}

template<typename T>
void dag<T>::remove_edge(const T& source, const T& destination) {
    if (adjacency_list.find(source) != adjacency_list.end()) {
        adjacency_list[source].erase(destination);
    }
}

template<typename T>
bool dag<T>::has_edge(const T& source, const T& destination) const {
    auto it = adjacency_list.find(source);
    if (it == adjacency_list.end()) {
        return false;
    }
    return it->second.find(destination) != it->second.end();
}

template<typename T>
bool dag<T>::has_node(const T& node) const {
    return nodes.find(node) != nodes.end();
}

template<typename T>
std::vector<T> dag<T>::get_nodes() const {
    return std::vector<T>(nodes.begin(), nodes.end());
}

template<typename T>
std::vector<T> dag<T>::get_neighbors(const T& node) const {
    std::vector<T> neighbors;
    auto it = adjacency_list.find(node);
    if (it != adjacency_list.end()) {
        neighbors.assign(it->second.begin(), it->second.end());
    }
    return neighbors;
}

template<typename T>
std::vector<T> dag<T>::get_predecessors(const T& node) const {
    std::vector<T> predecessors;
    for (const auto& pair : adjacency_list) {
        if (pair.second.find(node) != pair.second.end()) {
            predecessors.push_back(pair.first);
        }
    }
    return predecessors;
}

template<typename T>
bool dag<T>::has_cycle_dfs(const T& node, std::unordered_set<T>& visited, 
                           std::unordered_set<T>& rec_stack) const {
    visited.insert(node);
    rec_stack.insert(node);
    
    auto it = adjacency_list.find(node);
    if (it != adjacency_list.end()) {
        for (const T& neighbor : it->second) {
            if (visited.find(neighbor) == visited.end()) {
                if (has_cycle_dfs(neighbor, visited, rec_stack)) {
                    return true;
                }
            } else if (rec_stack.find(neighbor) != rec_stack.end()) {
                return true;
            }
        }
    }
    
    rec_stack.erase(node);
    return false;
}

template<typename T>
bool dag<T>::has_cycle() const {
    std::unordered_set<T> visited;
    std::unordered_set<T> rec_stack;
    
    for (const T& node : nodes) {
        if (visited.find(node) == visited.end()) {
            if (has_cycle_dfs(node, visited, rec_stack)) {
                return true;
            }
        }
    }
    return false;
}

template<typename T>
void dag<T>::topological_sort_dfs(const T& node, std::unordered_set<T>& visited, 
                                  std::stack<T>& result) const {
    visited.insert(node);
    
    auto it = adjacency_list.find(node);
    if (it != adjacency_list.end()) {
        for (const T& neighbor : it->second) {
            if (visited.find(neighbor) == visited.end()) {
                topological_sort_dfs(neighbor, visited, result);
            }
        }
    }
    
    result.push(node);
}

template<typename T>
std::vector<T> dag<T>::topological_sort() const {
    if (has_cycle()) {
        throw std::runtime_error("Cannot perform topological sort on a graph with cycles");
    }
    
    std::unordered_set<T> visited;
    std::stack<T> result;
    
    for (const T& node : nodes) {
        if (visited.find(node) == visited.end()) {
            topological_sort_dfs(node, visited, result);
        }
    }
    
    std::vector<T> sorted;
    while (!result.empty()) {
        sorted.push_back(result.top());
        result.pop();
    }
    
    return sorted;
}

template<typename T>
size_t dag<T>::size() const {
    return nodes.size();
}

template<typename T>
bool dag<T>::empty() const {
    return nodes.empty();
}

template<typename T>
void dag<T>::clear() {
    nodes.clear();
    adjacency_list.clear();
}

template<typename T>
void dag<T>::print() const {
    std::cout << "DAG Structure:\n";
    for (const T& node : nodes) {
        std::cout << node << " -> ";
        auto it = adjacency_list.find(node);
        if (it != adjacency_list.end() && !it->second.empty()) {
            bool first = true;
            for (const T& neighbor : it->second) {
                if (!first) std::cout << ", ";
                std::cout << neighbor;
                first = false;
            }
        } else {
            std::cout << "[]";
        }
        std::cout << "\n";
    }
}

template<typename T>
int dag<T>::get_in_degree(const T& node) const {
    int inDegree = 0;
    for (const auto& pair : adjacency_list) {
        if (pair.second.find(node) != pair.second.end()) {
            inDegree++;
        }
    }
    return inDegree;
}

template<typename T>
int dag<T>::get_out_degree(const T& node) const {
    auto it = adjacency_list.find(node);
    if (it == adjacency_list.end()) {
        return 0;
    }
    return it->second.size();
}

// Explicit template instantiations for common types
template class dag<int>;
template class dag<std::string>;
template class dag<char>;
