// Template implementations for dag<T>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <stack>
#include <algorithm>
#include <stdexcept>

template <typename T>
bool DAG<T>::has_cycle_helper(const T& node,
							  std::unordered_set<T> &visiting,
							  std::unordered_set<T> &visited) const {
	if (visiting.find(node) != visiting.end())
		return 1;

	if (visited.find(node) != visited.end())
		return 0;

	visiting.insert(node);

	auto it = adj.find(node);

	if (it == adj.end())
		return 0;

	for (const T& neighbor : it->second) {
		if (this->has_cycle_helper(neighbor, visiting, visited))
			return 1;
	}

	visiting.erase(node);
	visited.insert(node);

	return 0;
}

template <typename T>
bool DAG<T>::has_cycle() const {
	std::unordered_set<T> visiting;
	std::unordered_set<T> visited;

	for (const T& node : nodes) {
		if (visited.find(node) == visited.end()) {
			if (has_cycle_helper(node, visiting, visited))
				return 1;
		}
	}

	return 0;
}

template <typename T>
void DAG<T>::add_node(const T& node) {
	nodes.insert(node);

	if (adj.find(node) == adj.end()) {
		adj[node] = std::unordered_set<T>();
	}
}

template <typename T>
void DAG<T>::remove_node(const T& node) {
	if (nodes.find(node) == nodes.end())
		return;

	for (auto &p : adj) {
		p.second.erase(node);
	}

	nodes.erase(node);
	adj.erase(node);
}

template <typename T>
void DAG<T>::add_edge(const T& src, const T& dest) {
	// add if doesn't exist
	bool src_e = (nodes.find(src) != nodes.end()),
		 dest_e = (nodes.find(dest) != nodes.end());

	if (!src_e)
		this->add_node(src);

	if (!dest_e)
		this->add_node(dest);

	adj[src].insert(dest);

	if (this->has_cycle()) {
		adj[src].erase(dest);

		if (!src_e)
			this->remove_node(src);

		if (!dest_e)
			this->remove_node(dest);

		throw std::invalid_argument("Adding edge creates a cycle in the DAG");
	}
}

template <typename T>
void DAG<T>::remove_edge(const T& src, const T& dest) {
	if (adj.find(src) != adj.end()) {
		adj[src].erase(dest);
	}
}

template <typename T>
bool DAG<T>::has_edge(const T& src, const T& dest) const {
	auto it = adj.find(src);

	if (it == adj.end())
		return 0;

	return (it->second.find(dest) != it->second.end());
}

template <typename T>
bool DAG<T>::has_node(const T& node) const {
	return (nodes.find(node) != nodes.end());
}

template <typename T>
std::vector<T> DAG<T>::get_nodes() const {
	return std::vector<T>(nodes.begin(), nodes.end());
}

// out
template <typename T>
std::vector<T> DAG<T>::get_neighbors(const T& node) const {
	if (!this->has_node(node))
		return {};

	std::vector<T> neighbors;

	auto it = adj.find(node);

	if (it != adj.end()) {
		neighbors.assign(it->second.begin(), it->second.end());
	}

	return neighbors;
}

// in
template <typename T>
std::vector<T> DAG<T>::get_predecessors(const T& node) const {
	std::vector<T> pred;

	for (const auto &p : adj) {
		if (p.second.find(node) != p.second.end()) {
			pred.push_back(p.first);
		}
	}

	return pred;
}

// Kahn's Algorithm
template <typename T>
std::vector<T> DAG<T>::topological_sort() const {
	std::vector<T> L = {};
	std::stack<T> S = {};

	for (T node : nodes) {
		if (!this->get_indegree(node)) {
			S.push(node);
		}
	}

	// copy of edges
	std::unordered_map<T, std::unordered_set<T>> new_adj = adj;

	while (!S.empty()) {
		T top = S.top();
		S.pop();
		L.push_back(top);

		auto it = new_adj.find(top);

		if (it == new_adj.end()) {
			continue;
		}

		for (T n : it->second) {
			int in_n = 0;
			for (const auto &p : new_adj) {
				if (p.second.find(n) != p.second.end()) {
					in_n++;
				}
			}

			new_adj[top].erase(n);

			if (!in_n)
				S.push(n);
		}
	}

	return L;
}

template <typename T>
size_t DAG<T>::size() const {
	return nodes.size();
}

template <typename T>
bool DAG<T>::is_empty() const {
	return (this->size() == 0);
}

template <typename T>
void DAG<T>::clear() {
	nodes.clear();
	adj.clear();
}

template <typename T>
int DAG<T>::get_indegree(const T& node) const {
	int in = 0;

	for (const auto &p : adj) {
		if (p.second.find(node) != p.second.end()) {
			++in;
		}
	}

	return in;
}

template <typename T>
int DAG<T>::get_outdegree(const T& node) const {
	auto it = adj.find(node);

	if (it == adj.end())
		return 0;

	return it->second.size();
}


