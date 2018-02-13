#ifndef __GRAPH_H_INCLUDED__
#define __GRAPH_H_INCLUDED__

class Graph {
public:
	// Number of vertices
	int nVertices;

	// Number of edges
	int nEdges;

	// Adjacency matrix
	int **adj;

	// Vertex degree
	int *degree;

	// Vertex label
	int *label;

	Graph(int nVertices) {
		this -> nVertices = nVertices;
		nEdges = 0;
		adj = new int* [this -> nVertices];
		for (int v = 0; v < this -> nVertices; ++v) {
			adj[v] = new int [this -> nVertices];
		}
		degree = new int [this -> nVertices];
		label = new int [this -> nVertices];
		for (int u = 0; u < this -> nVertices; ++u) {
			for (int v = 0; v < this -> nVertices; ++v) {
				adj[u][v] = 0;
			}
			degree[u] = 0;
			label[u] = -1; // -1 denotes that this vertex is not label
		}
	}
};

#endif