#ifndef __GRAPHISOMORPHISM_H_INCLUDED__
#define __GRAPHISOMORPHISM_H_INCLUDED__

#include <algorithm>
#include <assert.h>

#include "Graph.h"

using namespace std;

class GraphIsomorphism {
public:
	GraphIsomorphism(int max_nVertices) {
		assert(max_nVertices > 0);
		this -> max_nVertices = max_nVertices;

		degree1 = new int [this -> max_nVertices];
		degree2 = new int [this -> max_nVertices];
		label1 = new int [this -> max_nVertices];
		label2 = new int [this -> max_nVertices];
		visited = new bool [this -> max_nVertices];
		used = new bool [this -> max_nVertices];
		perm = new int [this -> max_nVertices];
	}

	// Depth-First-Search
	void DFS(Graph *graph, int v) {
		visited[v] = true;
		for (int u = 0; u < graph -> nVertices; ++u) {
			if ((!visited[u]) && (graph -> adj[u][v] > 0)) {
				DFS(graph, u);
			}
		}
	}

	// Function to check if a given graph is connected by DFS
	bool is_connected_graph(Graph *graph) {
		for (int v = 0; v < graph -> nVertices; ++v) {
			visited[v] = false;
		}
		DFS(graph, 0);
		for (int v = 0; v < graph -> nVertices; ++v) {
			if (!visited[v]) {
				return false;
			}
		}
		return true;
	}


	// Check if the permutation is good
	bool check_permutation(Graph *graph1, Graph *graph2) {
		assert(graph1 -> nVertices == graph2 -> nVertices);

		for (int u = 0; u < graph1 -> nVertices; ++u) {
			int perm_u = perm[u];
			for (int v = 0; v < graph2 -> nVertices; ++v) {
				int perm_v = perm[v];
				if (graph1 -> adj[u][v] != graph2 -> adj[perm_u][perm_v]) {
					return false;
				}
			}
		}

		return true;
	}

	// Search for a permutation 
	void search_permutation(Graph *graph1, Graph *graph2, int v) {
		assert(graph1 -> nVertices == graph2 -> nVertices);

		for (int i = 0; i < graph1 -> nVertices; ++i) {
			if ((!used[i]) && (graph1 -> label[v] == graph2 -> label[i]) && (graph1 -> degree[v] == graph2 -> degree[i])) {
				perm[v] = i;
				used[i] = true;
				if (v + 1 < graph1 -> nVertices) {
					search_permutation(graph1, graph2, v + 1);
				} else {
					if (check_permutation(graph1, graph2)) {
						found = true;
						return;
					}
				}
				used[i] = false;
				if (found) {
					return;
				}
			}
		}
	}

	// Function to check if two input graphs are isomorphic
	bool are_isomorphic_graphs(Graph *graph1, Graph *graph2) {
		assert(graph1 -> nVertices <= max_nVertices);
		assert(graph2 -> nVertices <= max_nVertices);

		// Compare number of vertices
		if (graph1 -> nVertices != graph2 -> nVertices) {
			return false;
		}

		// Compare number of edges
		if (graph1 -> nEdges != graph2 -> nEdges) {
			return false;
		}

		// Compare the degree
		for (int v = 0; v < graph1 -> nVertices; ++v) {
			degree1[v] = graph1 -> degree[v];
			degree2[v] = graph2 -> degree[v];
		}

		sort(degree1, degree1 + graph1 -> nVertices);
		sort(degree2, degree2 + graph2 -> nVertices);

		for (int v = 0; v < graph1 -> nVertices; ++v) {
			if (degree1[v] != degree2[v]) {
				return false;
			}
		}

		// Compare the vertex label
		for (int v = 0; v < graph1 -> nVertices; ++v) {
			label1[v] = graph1 -> label[v];
			label2[v] = graph2 -> label[v];
		}

		sort(label1, label1 + graph1 -> nVertices);
		sort(label2, label2 + graph2 -> nVertices);

		for (int v = 0; v < graph1 -> nVertices; ++v) {
			if (label1[v] != label2[v]) {
				return false;
			}
		}

		// Search for a permutation
		for (int v = 0; v < graph1 -> nVertices; ++v) {
			used[v] = false;
		}
		found = false;
		search_permutation(graph1, graph2, 0);
		return found;
	}

	// Maximum number of vertices
	int max_nVertices;

	// A temporary array
	int *degree1;
	int *degree2;
	bool *visited;
	bool *used;
	int *perm;
	int *label1;
	int *label2;
	bool found;
};

#endif