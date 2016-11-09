//	Author:		Garry Griggs and Gregory Hughes
//	Date:		October 24th 2016
//	Project:	A* algorithm
//	File:		MiddleEarthGraph.h
//	Purpose:	This header file contatins the specifications for the graph's
//					methods

///////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <time.h>
#include <fstream>
#include <string>
#include <sstream>
#include <functional>
#include <queue>
#include <vector>
#include "Heuristics.h"


using namespace std;

const string GOAL_LOCATION = "Iron Hills";
const double TOTAL_PHEREMONE = 100;
const int CYCLES = 50;
const int TOTAL_ANTS = 10;

const int MAX_LOCATIONS = 25;

class Graph {

private:
	
	// Edge with opposite location, distance, road quality and risk level 
	struct Edge {
		string to;
		int distance;
		double pheremone;
		double probability;
	};

	// Node with current location distance to GOAL_LOCATION and a list of edges as well as edge count
	struct Node {
		string locationName;
		int distanceToIronHills;
		int edgeCount;
		Edge edges[8];
		bool visited;
	};

	struct Ant {
		int location;
		vector<int> actualPath;
		vector<int> path;
		int pathSum;	
	};
	
	
	Ant *ants;

	int locCounter;
	Node locations[MAX_LOCATIONS];

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
	// Post-Condition: addes an edge to the from node
	void addEdge(string from, string to, int d);

	// Parameters: node - string name of the node wanted
	// Returns: the node with the name passed in
	int getNode(string node);

	// Operator overloader for a priority_queue to make it a min heap
	struct CompareEdge : public binary_function<Edge, Edge, bool> {
		bool operator()(const Edge lhs, const Edge rhs) const {
			return lhs.probability < rhs.probability;
		}
	};

	// max heap for grabbing smallest fn paths
	

	double alpha, beta, rho;

	void firstMove();

	void initializeAnts();	
	void antCycles();
	bool allAntsAtGoal();
	bool antAtGoal(int i);
	void antsMove();
	void antMove(int ant);
	void updateAnt(int ant, int newLocation, int newPathSum);
	bool containsNode(int ant, int to);

	void addPheremones();
	double getPheremone(int pathSum);
	void addPheremone(int current, int next, int pathSum);
	void decayPheremones();
	void decayPheremone(int node, int edgeCount);
	void updateProbabilities();
	void updateProbability(int node, int edgeCount);
	double getSumOfWeights(int node, int edgeCount);
	Edge getNextEdge(int ant);
	bool isDeadEnd(int ant, Edge edge);

	int Graph::getDistance(int curr, int next);

public:
	// Graph Constuctor
	// Parameters: nodesWithDistances - take a specificly formatted file's name and 
	//                                  builds a graph with it
	Graph(string nodesWithDistances);	

	// Parameters: nodeConnData - a file of a specific format for adding edges
	void addEdges(string nodeConnData);

	//
	void runACO(double alpha, double beta, double rho);
	
	// Post-Condition: Builds a string of all the node data and prints
	// Returns: printLocs - the constructed string of data
	string toString();

};