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
const double TOTAL_PHEREMONE = 100;
const int CYCLES = 50;
const int TOTAL_ANTS = 10;

const int MAX_LOCATIONS = 25;

class Graph {

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

	// holds the values for each ant
	struct Ant {
		int location;
		vector<int> actualPath;
		vector<int> path;
		int pathSum;
	};

	// Operator overloader for a priority_queue to make it a min heap
	struct CompareEdge : public binary_function<Edge, Edge, bool> {
		bool operator()(const Edge lhs, const Edge rhs) const {
			return lhs.probability < rhs.probability;
		}
	};

	/////////////////////////////////////////////////////////////////////////
	//                        Private Data Members                         //
	/////////////////////////////////////////////////////////////////////////
	
	// PDMs for alpha beta and rho
	double alpha, beta, rho;

	// PDM to hold ants
	Ant *ants;

	// counter to hold location
	int locCounter;

	// Array of traveled locations
	Node locations[MAX_LOCATIONS];

	/////////////////////////////////////////////////////////////////////////
	//                       Private Methods                               //
	/////////////////////////////////////////////////////////////////////////
	
	// Initialize ants PDM
	void initializeAnts();

	// cyle ants on map
	void antCycles();
	
	// Initializes first move of ants
	void firstMove();

	// move ants
	void antsMove();

	// move ant based on integer location 
	void antMove(int ant);
		
	// returns the next edge for a given ant
	Edge getNextEdge(int ant);
	
	// returns the distance
	int getDistance(int curr, int next);
	
	// predicates if a given ant on a given edge is at a dead end
	bool isDeadEnd(int ant, Edge edge);
	
	// udate the given ant with new location and path sum
	void updateAnt(int ant, int newLocation, int newPathSum);
	
	// increases pheromone level accross the map
	void addPheremones();

	// increases pheromone for the path
	void addPheremone(int current, int next, int pathSum);

	// decays pheromone on the map
	void decayPheremones();

	// decay pheromone for the path
	void decayPheremone(int node, int edgeCount);

	// calculates the new probability
	void updateProbabilities();

	// updates probability for a given node
	void updateProbability(int node, int edgeCount);

	// returns the sum of the weights
	double getSumOfWeights(int node, int edgeCount);

	// returns the phermone 
	double getPheremone(int pathSum);

	// predicates if an ants path contains a node(city)
	bool containsNode(int ant, int to);

	// predicates if all ants in PDM are at goal node
	bool allAntsAtGoal();

	// predicates if given ant i is at goal node
	bool antAtGoal(int i);

	// Parameters : from - the current location
	//                to - the location the edge goes to
	//                 d - the distance of the edge
	// Post-Condition: addes an edge to the from node
	void addEdge(string from, string to, int d);

	// Parameters: s - a string to convert to an int
	// Returns: an int that was converted from a string
	int stringToInt(string s);

	// Returns: int representing the amount of nodes in graph
	int getSize();

	// Parameters: node - string name of the node wanted
	// Returns: the node with the name passed in
	int getNode(string node);

	// Parameters: location - the name of the node being added
	//             distance - the distance from that current node
	//                        to the destination
	// Post-Condition: creates a new node adds it to the locations list
	//                 and increments locCounter
	void addNode(string location, int distance);

	// Parameters: i - the int to be converted to a string
	// Returns: the string of a passed in int
	string intToString(int i);

};