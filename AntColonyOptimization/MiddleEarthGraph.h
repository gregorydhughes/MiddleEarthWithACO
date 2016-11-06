//	Author:		Garry Griggs and Gregory Hughes
//	Date:		October 24th 2016
//	Project:	A* algorithm
//	File:		MiddleEarthGraph.h
//	Purpose:	This header file contatins the specifications for the graph's
//					methods

///////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <functional>
#include <queue>
#include <vector>
#include "Heuristics.h"


using namespace std;

const string GOAL_LOCATION = "Iron Hills";

class Graph {	

private:

	// Edge with opposite location, distance, road quality and risk level 
	struct Edge {
		string to;
		int distance, roadQuality, riskLevel;
	};

	// Node with current location distance to GOAL_LOCATION and a list of edges as well as edge count
	struct Node {
		string locationName;
		int distanceToIronHills;
		int edgeCount;
		Edge edges[8];
		bool visited;
	};

	struct Path {
		int location;
		vector<int> path;
		int pathSum;
		int fn;		
	};

	struct ComparePath : public binary_function<Path*, Path*, bool> {
		bool operator()(const Path* lhs, const Path* rhs) const {
			return lhs->fn > rhs->fn;
		}
	};

	Path* finalPath;

	int pathCounter;
	priority_queue<Path*, vector<Path*>, ComparePath> paths;

	int locCounter;
	Node locations[25];

	// Parameters: i - the int to be converted to a string
	// Returns: the string of a passed in int
	string intToString(int i);

	// Parameters: s - a string to convert to an int
	// Returns: an int that was converted from a string
	int stringToInt(string s);

	// Parameters: location - the name of the node being added
	//             distance - the distance from that current node
	//                        to the destination
	// Post-Condition: creates a new node adds it to the locations list
	//                 and increments locCounter
	void addNode(string location, int distance);

	// Returns: int representing the amount of nodes in graph
	int getSize();

	// Parameters : from - the current location
	//                to - the location the edge goes to
	//                 d - the distance of the edge
	//                rq - the road quality of the edge
	//                rl - the risk level of the edge
	// Post-Condition: addes an edge to the from node
	void addEdge(string from, string to, int d, int rq, int rl);

	// Parameters: node - string name of the node wanted
	// Returns: the node with the name passed in
	int getNode(string node);

	int calculateFn(int g, int h);
	int getHeuristic(Edge e, Heuristics h);

	void addPath(int loc, int fn, int heur, int parent, const Path* curr);


public:
	// Graph Constuctor
	// Parameters: nodesWithDistances - take a specificly formatted file's name and 
	//                                  builds a graph with it
	Graph(string nodesWithDistances);	

	// Parameters: nodeConnData - a file of a specific format for adding edges
	void addEdges(string nodeConnData);


	void aStar(string start, Heuristics h);
	

	// Post-Condition: Builds a string of all the node data and prints
	// Returns: printLocs - the constructed string of data
	string toString();

};