#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <algorithm>
#include <assert.h>

#include "Graph.h"
#include "GraphIsomorphism.h"

using namespace std;

// Training set - input graphs and labels
const string train_graph_fn = "HCEP-50K/50K.train.graph";
const string train_atoms_fn = "HCEP-50K/50K.train.atoms";

// Training set - output dk-series features
string train_features_fn = "HCEP-50K-dKSeries/50K.train.dk-features.";

// Testing set - input graphs and labels
const string test_graph_fn = "HCEP-50K/50K.test.graph";
const string test_atoms_fn = "HCEP-50K/50K.test.atoms";

// Testing set - output dk-series features
string test_features_fn = "HCEP-50K-dKSeries/50K.test.dk-features.";

// Validation set - input graphs and labels
const string val_graph_fn = "HCEP-50K/50K.val.graph";
const string val_atoms_fn = "HCEP-50K/50K.val.atoms";

// Validation set - output dk-series features
string val_features_fn = "HCEP-50K-dKSeries/50K.val.dk-features.";

// Basic atoms
const string basics_fn = "HCEP-50K/50K.basics";

// Output all non-isomorphic graphs
string non_isomorphic_graphs_fn = "HCEP-50K-dKSeries/50K.non_isomorphic_graphs.";

// Maximum number of vertices in a subgraph
int d;

// Maximum number of vertices
int max_nVertices;

// Molecule structure
struct Molecule {
	Graph *graph;
	vector<string> atoms;
	vector<int> dk_index;
	int *dk_feature;
};

// Training set
int nTrain;
Molecule **train;

// Testing set
int nTest;
Molecule **test;

// Validation set
int nVal;
Molecule **val;

// All basic atoms
vector<string> basic_atoms;

// All non-isomorphic graphs
vector<Graph*> graphs;
int nFeatures;

// Data structure to keep track of the combination of vertices
int *subgraph;

// Reference graph
Graph *reference;

// Graph Isomorphism
GraphIsomorphism *iso;

// Function to pair a string into a vector of words
vector<string> separate_words(string line) {
	vector<string> ret;
	ret.clear();
	int i = 0;
	while (i < line.length()) {
		if (line[i] == ' ') {
			++i;
			continue;
		}
		string word = "";
		for (int j = i; j < line.length(); ++j) {
			if (line[j] != ' ') {
				word += line[j];
				i = j;
			} else {
				break;
			}
		}
		++i;
		ret.push_back(word);
	}
	return ret;
}

// Memory allocation
void memory_allocation() {
	ifstream file;

	file.open(train_graph_fn.c_str(), ios::in);
	file >> nTrain;
	file.close();

	file.open(test_graph_fn.c_str(), ios::in);
	file >> nTest;
	file.close();

	file.open(val_graph_fn.c_str(), ios::in);
	file >> nVal;
	file.close();

	train = new Molecule* [nTrain];
	test = new Molecule* [nTest];
	val = new Molecule* [nVal];

	for (int i = 0; i < nTrain; ++i) {
		train[i] = new Molecule();
	}

	for (int i = 0; i < nTest; ++i) {
		test[i] = new Molecule();
	}

	for (int i = 0; i < nVal; ++i) {
		val[i] = new Molecule();
	}

	subgraph = new int [d];
	reference = new Graph(d);
	iso = new GraphIsomorphism(d);
	graphs.clear();

	cout << "Number of training molecules: " << nTrain << endl;
	cout << "Number of validation molecules: " << nVal << endl;
	cout << "Number of testing molecules: " << nTrain << endl;
}

// Reading basic atoms
void read_basic_atoms() {
	int number;
	string line;

	ifstream file(basics_fn.c_str(), ios::in);
	getline(file, line);
	number = atoi(line.c_str());
	basic_atoms.clear();
	for (int i = 0; i < number; ++i) {
		getline(file, line);
		basic_atoms.push_back(line);
	}
	file.close();

	cout << "Number of basic atoms: " << basic_atoms.size() << endl;
	cout << "Basic atoms:" << endl;
	for (int i = 0; i < basic_atoms.size(); ++i) {
		cout << basic_atoms[i] << endl;
	}
}

// Return atom label
int atom_label(string label) {
	for (int i = 0; i < basic_atoms.size(); ++i) {
		if (basic_atoms[i] == label) {
			return i;
		}
	}
	return -1;
}

// Reading data: graph and atoms
void read_data(string graph_fn, string atoms_fn, Molecule **data, int nData) {
	int number;
	string line;

	cout << "Reading graph file " << graph_fn << endl;
	ifstream graph(graph_fn.c_str(), ios::in);
	graph >> number;
	assert(number == nData);
	for (int i = 0; i < nData; ++i) {
		graph >> number;
		max_nVertices = max(max_nVertices, number);
		data[i] -> graph = new Graph(number);
		for (int u = 0; u < data[i] -> graph -> nVertices; ++u) {
			for (int v = 0; v < data[i] -> graph -> nVertices; ++v) {
				graph >> number;
				data[i] -> graph -> add_edge(u, v, number);
			}
		}
		data[i] -> graph -> update_degree();
	}
	graph.close();

	cout << "Reading atoms file " << atoms_fn << endl;
	ifstream atoms(atoms_fn.c_str(), ios::in);
	getline(atoms, line);
	number = atoi(line.c_str());
	assert(number == nData);
	for (int i = 0; i < nData; ++i) {
		getline(atoms, line);
		number = atoi(line.c_str());
		assert(number == data[i] -> graph -> nVertices);
		getline(atoms, line);
		data[i] -> atoms = separate_words(line);
		assert(data[i] -> atoms.size() == data[i] -> graph -> nVertices);
		for (int v = 0; v < data[i] -> graph -> nVertices; ++v) {
			int label = atom_label(data[i] -> atoms[v]);
			assert(label != -1);
			data[i] -> graph -> set_label(v, label);
		}
	}
	atoms.close();
}

// Check the new subgraph
void update(Molecule *molecule, int n) {
	// Construct the subgraph
	reference -> nVertices = n;
	for (int u = 0; u < n; ++u) {
		int mapped_u = subgraph[u];
		int label = molecule -> graph -> get_label(subgraph[u]);
		reference -> set_label(u, label);
		for (int v = 0; v < n; ++v) {
			int mapped_v = subgraph[v];
			int adj = molecule -> graph -> adj[mapped_u][mapped_v];
			reference -> add_edge(u, v, adj);
		}
		reference -> update_degree();
	}

	// Check if the referene graph is connected
	if (!iso -> is_connected_graph(reference)) {
		return;
	}

	// Check if the subgraph has an isomorphic one in the list of graphs
	int dk_index = -1;
	for (int i = 0; i < graphs.size(); ++i) {
		if (iso -> are_isomorphic_graphs(graphs[i], reference)) {
			dk_index = i;
			break;
		}
	}

	// If not found, add the new subgraph into the list
	if (dk_index == -1) {
		Graph *g = new Graph(n);
		for (int u = 0; u < n; ++u) {
			for (int v = 0; v < n; ++v) {
				g -> adj[u][v] = reference -> adj[u][v];
			}
			g -> degree[u] = reference -> degree[u];
			g -> label[u] = reference -> label[u];
		}
		graphs.push_back(g);
		dk_index = graphs.size() - 1;
	}

	// Add the new index to the molecule
	molecule -> dk_index.push_back(dk_index);
}

// Back-Tracking to find all combination
void Back_Tracking(Molecule *molecule, int v, int nSelected) {
	for (int state = 0; state < 2; ++state) {
		/*
		// Condition to select exactly d vertices
		if (nSelected + state + (molecule -> graph -> nVertices - v - 1) < d) {
			continue;
		}
		*/
		if (nSelected + state > d) {
			continue;
		}
		if (state == 1) {
			subgraph[nSelected] = v;
			++nSelected;
		}
		if ((v == molecule -> graph -> nVertices - 1) && (nSelected > 0)) {
			update(molecule, nSelected);
		}
		if (v + 1 < molecule -> graph -> nVertices) {
			Back_Tracking(molecule, v + 1, nSelected);
		}
		if (state == 1) {
			--nSelected;
		}
	}
}

// dK feature extraction
void dk_feature_extraction(Molecule **data, int nData) {
	for (int i = 0; i < nData; ++i) {
		Back_Tracking(data[i], 0, 0);
		if ((i + 1) % 1000 == 0) {
			cout << "    Done " << (i + 1) << "/" << nData << " molecules" << endl;
		}
	}
}

// Write all non-isomorphic graphs to file
void output() {
	cout << "Save all non-isomorphic graphs to file " << non_isomorphic_graphs_fn << endl;
	ofstream file(non_isomorphic_graphs_fn.c_str(), ios::out);
	for (int i = 0; i < graphs.size(); ++i) {
		file << "Non-isomorphic subgraph " << (i + 1) << ":" << endl;
		file << "Number of vertices = " << endl;
		file << graphs[i] -> nVertices << endl;
		file << "Adjacency = " << endl;
		for (int u = 0; u < graphs[i] -> nVertices; ++u) {
			for (int v = 0; v < graphs[i] -> nVertices; ++v) {
				file << graphs[i] -> adj[u][v] << " ";
			}
			file << endl;
		}
		file << "Label = " << endl;
		for (int v = 0; v < graphs[i] -> nVertices; ++v) {
			file << graphs[i] -> label[v] << " ";
		}
		file << endl;
		file << "Atom type = " << endl;
		for (int v = 0; v < graphs[i] -> nVertices; ++v) {
			file << basic_atoms[graphs[i] -> label[v]] << " ";
		}
		file << endl;
	}
	file.close();
}

// Save dk-features
void save_dk_features(Molecule **data, int nData, string feature_fn) {
	ofstream feature(feature_fn.c_str(), ios::out);
	for (int i = 0; i < nData; ++i) {
		data[i] -> dk_feature = new int [nFeatures];
		for (int index = 0; index < nFeatures; ++index) {
			data[i] -> dk_feature[index] = 0;
		}
		for (int j = 0; j < data[i] -> dk_index.size(); ++j) {
			int index = data[i] -> dk_index[j];
			++data[i] -> dk_feature[index];
		} 
		for (int index = 0; index < nFeatures; ++index) {
			feature << data[i] -> dk_feature[index] << " ";
		}
		feature << endl;
	}
	feature.close();
}

// Main function
int main(int argc, char **argv) {
	if (argc != 2) {
		cerr << "Input exactly 1 parameter: d" << endl;
		return 0;
	}
	
	train_features_fn += string(argv[1]);
	test_features_fn += string(argv[1]);
	val_features_fn += string(argv[1]);
	non_isomorphic_graphs_fn += string(argv[1]);

	d = atoi(argv[1]);
	if (d >= 4) {
		cout << "Warning: Large d >= 4 takes a long time!" << endl;
	}
	
	max_nVertices = 0;
	memory_allocation();
	read_basic_atoms();
	read_data(train_graph_fn, train_atoms_fn, train, nTrain);
	read_data(test_graph_fn, test_atoms_fn, test, nTest);
	read_data(val_graph_fn, val_atoms_fn, val, nVal);
	cout << "Maximum number of vertices: " << max_nVertices << endl;

	cout << "dK feature extraction on the training set" << endl;
	dk_feature_extraction(train, nTrain);

	cout << "dK feature extraction on the testing set" << endl;
	dk_feature_extraction(test, nTest);

	cout << "dK feature extraction on the validation set" << endl;
	dk_feature_extraction(val, nVal);

	cout << "Number of non-isomorphic graphs found in all data: " << graphs.size() << endl;
	nFeatures = graphs.size();

	output();
	save_dk_features(train, nTrain, train_features_fn);
	save_dk_features(test, nTest, test_features_fn);
	save_dk_features(val, nVal, val_features_fn);
	return 0;
}