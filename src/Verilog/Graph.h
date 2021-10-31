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
	void startBFS();
	void generateForwardingTable();
	Component* findRoot();
	void initializeDFS();

private:
	DotReader dotReader;
	int maxDFSDepth = 500, currentDFSDepth = 0;
	std::map<std::string, bool> visited;//For DFS/BFS
	std::map<Component*, std::pair<Component*, int>> pred_dist;//For BFS

	std::string generateComponentForwardingTable(Component* v, int N);
	std::string generateComponentForwardingTableBFS(Component* v, int N);
	int getForwardingPort(Component* src, Component* dest, std::map<Component*, std::pair<Component*, int>> &pred_dist);


	void DFS(Component* v, short level, short outPort, short* arr);

	std::string decToBinary(short n, short size);

	bool BFS(Component* src, std::map<Component*, std::pair<Component*, int>> &pred_dist);
	void printShortestDistance(Component* src, Component* dest, std::map<Component*, std::pair<Component*, int>> &pred_dist);
};

#endif /* GRAPH_H_ */
