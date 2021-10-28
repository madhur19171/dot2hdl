/*
 * Graph.h
 *
 *  Created on: 30-May-2021
 *      Author: madhur
 */

#ifndef GRAPH_H_
#define GRAPH_H_

#pragma once

#include "dot_reader1.h"
#include <bitset>

class Graph {
public:
//	Graph();
	Graph(DotReader dotReader);
	void startDFS();
	void generateForwardingTable();
	Component* findRoot();
	void initializeDFS();

private:
	DotReader dotReader;
	int maxDFSDepth = 500, currentDFSDepth = 0;
	std::map<std::string, bool> visited;

	std::string generateComponentForwardingTable(Component* v, int N);

	void DFS(Component* v, short level, short outPort, short* arr);

	std::string decToBinary(short n, short size);

	bool BFS(vector<int> adj[], int src, int dest, int v, int pred[], int dist[]);
	void printShortestDistance(vector<int> adj[], int s, int dest, int v);
};

#endif /* GRAPH_H_ */
