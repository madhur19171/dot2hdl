/*
 * Graph.cpp
 *
 *  Created on: 30-May-2021
 *      Author: madhur
 */

#include "Graph.h"

Graph::Graph(DotReader dotReader) {
	// TODO Auto-generated constructor stub
	this->dotReader = dotReader;

}


//void Graph::startDFS(){
//
//	initializeDFS();//populates the visited map, otherwise it throws error
//
//	//Starts DFS from the "start_0"/"Node0"/first component
//	Component* root = findRoot();
//	DFS(root);
//
//	//This loop will make sure that all those components that have not yet been visited
//	//will also be traversed. Components which have no input will never be reached
//	//Eg. "const" has no input, so it has to be traversed separately.
//	//Additionally, the entry points are also one of the roots of the graph, so all the entry points
//	//Other than "start" are also traversed here
//	for(auto it = visited.begin(); it != visited.end(); it++){
//		Component* comp = dotReader.getComponentMap().at(it->first);
//		//		cout << comp->name << " " << comp->type << endl;
//		//If a component has no input or component is an entry point, and not visited yet.
//		if((comp->in.size == 0 || comp->type == COMPONENT_START) && !visited[comp->name]){
//			root = comp;
//			DFS(root);
//		}
//	}
//


void Graph::DFS(Component* v, short level, short outPort, short* arr){
	//CurrentDFSDepth keeps track of current DFS Level. For debugging purposes only
	if(currentDFSDepth++ < maxDFSDepth){
		visited[v->name] = true;
		std::cout << v->name << "_";


		Component* nextComponent;
		if(v->io.size() != 0){

			for(auto it = v->io.begin(); it != v->io.end(); it++){
				std::string name = ((*it).first)->name;
				if(level == 0)
					outPort = ((*it).second).first;
				nextComponent = (dotReader.getComponentMap().at(name));
				if(!visited[name]){
					arr[nodes[((*it).first)->index].node_id] = outPort;
					std::cout << ((*it).second).first << " -> " << ((*it).second).second << "_";
					DFS(nextComponent, level + 1, outPort, arr);
					currentDFSDepth--;
				}else{
					//					std::cout << std::endl;
				}
			}
			std::cout << std::endl;
		}else{
			//			std::cout << std::endl;
		}
	} else
		return;
}

//The graph starts with the component named "start_0"
Component* Graph::findRoot(){
	if(dotReader.getComponentMap().find("start_0") != dotReader.getComponentMap().end())
		return dotReader.getComponentMap().at("start_0");
	else if(dotReader.getComponentMap().find("Node0") != dotReader.getComponentMap().end())
		return dotReader.getComponentMap().at("Node0");
	else return dotReader.getComponentList().at(0);
}

//Initialize visited map.
void Graph::initializeDFS(){
	visited.clear();
	for(auto it = dotReader.getComponentList().begin(); it != dotReader.getComponentList().end(); it++){
		visited.insert({(*it)->name, false});
	}
	//	std::cout << "DFS Starts: " << std::endl;
}

//Generates forwarding table for Component v
std::string Graph::generateComponentForwardingTable(Component* v, int N){
	std::string ret = "";

	short entrySize = ceil(log2(v->out.size));

	short arr[N];

	initializeDFS();
	DFS(v, 0, 0, arr);
	arr[nodes[v->index].node_id] = 0;
	ret = decToBinary(arr[N - 1], entrySize);
	std::cout << "Decimal: " << arr[N - 1] << " ";
	for(int i = N - 2; i >= 0; i--){
		std::cout << arr[i] << " ";
		ret += "_" + decToBinary(arr[i], entrySize);
	}
	std::cout << endl;
	return ret;
}


//Generates forwarding table for Component v
std::string Graph::generateComponentForwardingTableBFS(Component* v, int N){
	std::string ret = "";

	short entrySize = ceil(log2(v->out.size));

	initializeDFS();
	BFS(v, pred_dist);

	std::cout << "Decimal: ";

	for(int i = N - 1; i >= 0; i--){//N - 2 for DFS
		int forwardingPort = getForwardingPort(v, dotReader.getComponentList()[i],pred_dist);
		std::cout << forwardingPort << " ";
		ret += (i == N - 1 ? "" : "_") + decToBinary(forwardingPort, entrySize);
	}
	std::cout << endl;
	return ret;
}

int Graph::getForwardingPort(Component* src, Component* dest, std::map<Component*, std::pair<Component*, int>> &pred_dist)
{
	int ret = 0;
	// vector path stores the shortest path
	vector<Component*> path;
	Component* crawl = dest;
	path.push_back(crawl);
	while (pred_dist[crawl].first != NULL) {
		path.push_back(pred_dist[crawl].first);
		crawl = pred_dist[crawl].first;
	}

	if(path.size() != 1){
		for(auto it = (src->io).begin(); it != (src->io).end(); it++)
			if(((*it).first) == path[path.size() - 2])
				ret = (*it).second.first;

	}

	return ret;
}



//Generates Forwarding Table for all Router Components.
void Graph::generateForwardingTable(){

	for(auto it = dotReader.getComponentList().begin(); it != dotReader.getComponentList().end(); it++){
		std::string str = generateComponentForwardingTableBFS(*it, dotReader.getComponentList().size());//Assuming that all components are Routers

		std::string file_n = (*it)->name;
		std::ofstream outStream(file_n);
		outStream << str << std::endl;
		std::cout << "Binary: " << str << std::endl;
		outStream.close();
	}
}

std::string Graph::decToBinary(short n, short size)
{
	std::string ret = "";
	// Size of an integer is assumed to be 32 bits
	for (int i = 31; i >= 0; i--) {
		int k = n >> i;
		if (k & 1)
			ret += "1";
		else
			ret += "0";
	}

	return ret.substr(ret.size() - size, size);
}



// a modified version of BFS that stores predecessor
// of each vertex in array p
// and its distance from source in array d
bool Graph::BFS(Component* src, std::map<Component*, std::pair<Component*, int>> &pred_dist)
{
	// a queue to maintain queue of vertices whose
	// adjacency list is to be scanned as per normal
	// DFS algorithm
	list<Component*> queue;

	// boolean array visited[] which stores the
	// information whether ith vertex is reached
	// at least once in the Breadth first search
	//    bool visited[N];
	initializeDFS();

	// initially all vertices are unvisited
	// so v[i] for all i is false
	// and as no path is yet constructed
	// dist[i] for all i set to infinity

	for(auto it = dotReader.getComponentList().begin(); it != dotReader.getComponentList().end(); it++){
		pred_dist.clear();//Empty the map before starting BFS
		std::pair<Component*, int> pair;
		pair.first = NULL;
		pair.second = -1;
		pred_dist.insert({(*it), pair});
	}

	// now source is first to be visited and
	// distance from source to itself should be 0
	visited[src->name] = true;
	pred_dist[src] = {NULL, 0};
	queue.push_back(src);

	// standard BFS algorithm
	while (!queue.empty()) {
		Component* u = queue.front();
		queue.pop_front();
		std::cout << u->name << ", ";
		for (auto it = (u->io).begin(); it != (u->io).end(); it++) {
			if (visited[((*it).first)->name] == false) {
				visited[((*it).first)->name] = true;

				pred_dist[((*it).first)].first = u;
				pred_dist[((*it).first)].second = pred_dist[u].second + 1;

				queue.push_back((*it).first);

				std::cout << (*it).first->name << ", ";

				// We stop BFS when we find
				// destination.
				//                if (adj[u][i] == dest)
				//                    return true;
			}
		}
		std::cout << std::endl;
	}

	return true;
}

// utility function to print the shortest distance
// between source vertex and destination vertex
void Graph::printShortestDistance(Component* src, Component* dest, std::map<Component*, std::pair<Component*, int>> &pred_dist)
{

	std::cout << "Source: " << src->name << "\tDest: " << dest->name << std::endl;
	// vector path stores the shortest path
	vector<Component*> path;
	Component* crawl = dest;
	path.push_back(crawl);
	while (pred_dist[crawl].first != NULL) {
		path.push_back(pred_dist[crawl].first);
		crawl = pred_dist[crawl].first;
	}

	// distance from source is in distance array
	cout << "Shortest path length is : " << pred_dist[dest].second << std::endl;

	// printing path from source to destination
	cout << "Path is::\n";
	for (int i = path.size() - 1; i >= 0; i--)
		cout << path[i]->name << " ";
	std::cout << std::endl;
}


void Graph::startBFS(){
	for(auto it = dotReader.getComponentList().begin(); it != dotReader.getComponentList().end(); it++){
		Component* src = *it;
		BFS(src, pred_dist);
		for(auto jt = dotReader.getComponentList().begin(); jt != dotReader.getComponentList().end(); jt++){
			if(*jt != *it)
				printShortestDistance(src, *jt, pred_dist);
		}
	}
}














