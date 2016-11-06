//	Author:		Garry Griggs and Gregory Hughes
//	Date:		October 24th 2016
//	Project:	A* algorithm
//	File:		MiddleEarthGraph.cpp
//	Purpose:	This cpp file cotnains the implementation of the graph's 
//					structure

///////////////////////////////////////////////////////////////////////////////

#include "MiddleEarthGraph.h"

using namespace std;

// Graph Constuctor
// Parameters: nodesWithDistances - take a specificly formatted file's name and 
//                                  builds a graph with it
Graph::Graph(string nodesWithDistances) {
	locCounter = 0;
	string currentNode;
	ifstream inFile;
	inFile.open(nodesWithDistances.c_str());

	finalPath = new Path;
	finalPath->fn = 0;
	finalPath->path.push_back(0);
	finalPath->location = 0;
	finalPath->pathSum = 0;

	// this loops builds the nodes line by line
	// it also does the parsing
	for (int i = 0; i < 25; i++) {
		getline(inFile, currentNode);
		string name = currentNode.substr(0, currentNode.find(" "));
		currentNode.erase(0, currentNode.find(" ") + 1);
		string temp;
		if (currentNode.find(" ") != -1) {
			name += " " + currentNode.substr(0, currentNode.find(" "));
			currentNode.erase(0, currentNode.find(" ") + 1);
		}
		addNode(name, stringToInt(currentNode));
	}
}

void Graph::aStar(string start, Heuristics h) {
	int currLoc = getNode(start);
	Path* curr = new Path;
	curr->fn = 0;
	curr->location = currLoc;
	curr->pathSum = 0;

	while (locations[curr->location].locationName.compare(GOAL_LOCATION) != 0) {
		
		int edges = locations[curr->location].edgeCount;
		for (int i = 0; i < edges; i++) {
			int to = getNode(locations[curr->location].edges[i].to);
			for (int j = 0; j < curr->path.size(); j++)
				if (curr->path[j] == to)
					continue;
			int dtih = locations[to].distanceToIronHills;
			int heur = getHeuristic(locations[curr->location].edges[i], h);
			int fn = calculateFn(dtih, heur);
			addPath(to, fn, heur, curr->location, curr);
		}
		delete curr;
		curr = paths.top();
		paths.pop();
	}
	finalPath = curr;
}

void Graph::addPath(int loc, int fn, int heur, int parent, const Path* curr) {
	Path* p = new Path;
	p->fn = fn;
	p->location = loc;
	p->path = curr->path;
	p->path.push_back(parent);
	p->pathSum = curr->pathSum + heur;
	paths.push(p);	
}

int Graph::calculateFn(int g, int h) {
	return g + h;
}

int Graph::getHeuristic(Edge e, Heuristics h) {
	switch (h) {
		case ROAD_QUALITY:
			return e.roadQuality;
			break;
		case DISTANCE:
			return e.distance;
			break;
		case RISK_LEVEL:
			return e.riskLevel;
			break;
		default:
			break;
	}
}

// Parameters : from - the current location
//                to - the location the edge goes to
//                 d - the distance of the edge
//                rq - the road quality of the edge
//                rl - the risk level of the edge
// Post-Condition: addes an edge to the from node
void Graph::addEdge(string from, string to, int d, int rq, int rl) {
	int temp = getNode(from);

	// I assume you want to do this for all the edges? and not just the edge count
	locations[temp].edges[locations[temp].edgeCount].to = to;
	locations[temp].edges[locations[temp].edgeCount].distance = d;
	locations[temp].edges[locations[temp].edgeCount].roadQuality = (rq * -1);
	locations[temp].edges[locations[temp].edgeCount].riskLevel = rl;
	locations[temp].edgeCount++;
}

// Parameters: nodeConnData - a file of a specific format for adding edges
void Graph::addEdges(string nodeConnData) {
	string from;
	ifstream inFile;
	inFile.open(nodeConnData.c_str());

	for (int t = 0; t < 23; t++) {
		getline(inFile, from);
		string total;
		getline(inFile, total);
		int x = stringToInt(total);
		int d, rq, rl;
		string to;

		for (int i = 0; i < x; i++) {
			getline(inFile, to);
			string temp;
			getline(inFile, temp);
			d = stringToInt(temp.substr(0, temp.find(" ")));
			temp.erase(0, temp.find(" ") + 1);
			rq = stringToInt(temp.substr(0, temp.find(" ")));
			temp.erase(0, temp.find(" ") + 1);
			rl = stringToInt(temp);
			addEdge(from, to, d, rq, rl);
			addEdge(to, from, d, rq, rl);
		}
	}
}

// Parameters: s - a string to convert to an int
// Returns: an int that was converted from a string
int Graph::stringToInt(string s) {
	int x;
	stringstream ss(s);
	ss >> x;
	return x;
}

// Returns: int representing the amount of nodes in graph
int Graph::getSize() {
	return locCounter - 1;
}

// Parameters: node - string name of the node wanted
// Returns: the node with the name passed in
int Graph::getNode(string node) {
	for (int i = 0; i < locCounter; i++)
		if (locations[i].locationName.compare(node) == 0)
			return i;
}

// Parameters: location - the name of the node being added
//             distance - the distance from that current node
//                        to the destination
// Post-Condition: creates a new node adds it to the locations list
//                 and increments locCounter
void Graph::addNode(string location, int distance) {
	locations[locCounter].locationName = location;
	locations[locCounter].distanceToIronHills = distance;
	locations[locCounter].edgeCount = 0;
	locations[locCounter].visited = false;
	locCounter++;
}

// Parameters: i - the int to be converted to a string
// Returns: the string of a passed in int
string Graph::intToString(int i) {
	stringstream ss;
	ss << i;
	return ss.str();
}

// Post-Condition: Builds a string of all the node data and prints
// Returns: printLocs - the constructed string of data
string Graph::toString() {
	string printLocs = "";
	printLocs += "Final Location: " + locations[finalPath->location].locationName + "\n";
	printLocs += "Final F(n): " + intToString(finalPath->fn) + "\n";
	printLocs += "Path Sum: " + intToString(finalPath->pathSum) + "\n";
	printLocs += "Final Path: ";
	for (int i = 0; i < finalPath->path.size(); i++)
		printLocs += locations[finalPath->path[i]].locationName + " -> ";

	return printLocs + locations[finalPath->location].locationName + "\n";
}