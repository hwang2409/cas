#ifndef DAG_HPP
#define DAG_HPP

#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <stack>
#include <algorithm>
#include <stdexcept>


template <typename T>
class DAG {
	private:
		std::unordered_map<T, std::unordered_set<T>> adj;
		std::unordered_set<T> nodes;

		// visited: set of nodes we've visited
		// rec: recursive stack
		bool has_cycle_helper(const T& node,
							  std::unordered_set<T> &visiting,
							  std::unordered_set<T> &visited) const;
	public:
		DAG() = default;

		~DAG() = default;

		bool has_cycle() const;

		void add_node(const T& node);

		void remove_node(const T& node);

		void add_edge(const T& src, const T& dest);

		void remove_edge(const T& src, const T& dest);

		bool has_edge(const T& src, const T& dest) const;

		bool has_node(const T& node) const;

		std::vector<T> get_nodes() const;

		std::vector<T> get_neighbors(const T& node) const;

		std::vector<T> get_predecessors(const T& node) const;

		std::vector<T> topological_sort() const;

		size_t size() const;

		bool is_empty() const;

		void clear();

		int get_indegree(const T& node) const;

		int get_outdegree(const T& node) const;
};

#include "dag.cpp"

#endif
