#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <algorithm>

#include "Graph.h"
#include "GraphIsomorphism.h"

using namespace std;

// Maximum number of vertices
int d;

// Graph Isomorphism 
GraphIsomorphism *test;

// List of non-isomorphic graphs
vector<Graph*> graphs;

// A reference graph for Back-Tracking algorithm
Graph *reference;

// Add a new graph to the list
void update_graphs() {
	if (!test -> is_connected_graph(reference)) {
		return;
	}

	for (int i = 0; i < graphs.size(); ++i) {
		if (test -> are_isomorphic_graphs(graphs[i], reference)) {
			return;
		}
	}

	// Create a new graph
	Graph *graph = new Graph(reference -> nVertices);
	graph -> nVertices = reference -> nVertices;
	graph -> nEdges = reference -> nEdges;
	for (int u = 0; u < reference -> nVertices; ++u) {
		for (int v = 0; v < reference -> nVertices; ++v) {
			graph -> adj[u][v] = reference -> adj[u][v];
		}
		graph -> degree[u] = reference -> degree[u];
	}

	// Add to the list of graphs
	graphs.push_back(graph);
}

// Back-Tracking algorithm to generate a graph
void Back_Tracking(int u, int v, int n) {
	for (int state = 0; state < 2; ++state) {
		reference -> adj[u][v] = state;
		reference -> adj[v][u] = state;
		reference -> degree[u] += state;
		reference -> degree[v] += state;
		reference -> nEdges += state;

		if (v + 1 < n) {
			Back_Tracking(u, v + 1, n);
		} else {
			if (u + 2 < n) {
				Back_Tracking(u + 1, u + 2, n);
			} else {
				update_graphs();
			}
		}

		reference -> adj[u][v] = 0;
		reference -> adj[v][u] = 0;
		reference -> degree[u] -= state;
		reference -> degree[v] -= state;
		reference -> nEdges -= state;
	}
}

// Generate all possible graphs of size n
void graphs_generation(int n) {
	if (n == 1) {
		Graph *graph = new Graph(1);
		graphs.push_back(graph);
		return;
	}

	// Empty the reference graph
	reference -> nVertices = n;
	reference -> nEdges = 0;
	for (int u = 0; u < n; ++u) {
		for (int v = 0; v < n; ++v) {
			reference -> adj[u][v] = 0;
		}
		reference -> degree[u] = 0;
	}

	Back_Tracking(0, 1, n);
}

// Generate all possible graphs of size <= d
void graphs_generation() {
	// Make a reference graph
	reference = new Graph(d);

	// Create the object to test for graph isomorphism
	test = new GraphIsomorphism(d);

	// Back-Tracking algorithm
	graphs.clear();
	for (int n = 1; n <= d; ++n) {
		graphs_generation(n);
	}
}

// Write the output
void output() {
	cout << "Number of graphs found: " << graphs.size() << endl;
	int i = 0;
	while (i < graphs.size()) {
		int j = i;
		for (int v = i; v < graphs.size(); ++v) {
			if (graphs[v] -> nVertices == graphs[i] -> nVertices) {
				j = v;
			} else {
				break;
			}
		}
		cout << "Number of graphs of size " << graphs[i] -> nVertices << ": " << (j - i + 1) << endl;
		i = j + 1;
	}

	for (i = 0; i < graphs.size(); ++i) {
		cout << "--- Graph " << (i + 1) << " -------------------" << endl;
		cout << "Number of vertices = " << graphs[i] -> nVertices << endl;
		cout << "Number of edges = " << graphs[i] -> nEdges << endl;
		cout << "Adjacency matrix = " << endl;
		for (int u = 0; u < graphs[i] -> nVertices; ++u) {
			for (int v = 0; v < graphs[i] -> nVertices; ++v) {
				cout << graphs[i] -> adj[u][v] << " ";
			}
			cout << endl;
		}
		cout << "Vertex degree = ";
		for (int v = 0; v < graphs[i] -> nVertices; ++v) {
			cout << graphs[i] -> degree[v] << " ";
		}
		cout << endl;
	}
}

// Main function
int main(int argc, char **argv) {
	if (argc != 2) {
		cerr << "Provide exactly 1 parameter: d." << endl;
		return 0;
	}
	d = atoi(argv[1]);
	if (d < 1) {
		cerr << "d >= 1" << endl;
		return 0;
	}
	if (d >= 6) {
		cout << "Warning: Large d takes a long time!" << endl;
	}
	graphs_generation();
	output();
	return 0;
}