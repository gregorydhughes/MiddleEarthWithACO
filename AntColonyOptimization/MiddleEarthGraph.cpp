//	Author:		Garry Griggs and Gregory Hughes
//	Date:		October 24th 2016
//	Project:	A* algorithm
//	File:		MiddleEarthGraph.cpp
//	Purpose:	This cpp file cotnains the implementation of the graph's 
//					structure

///////////////////////////////////////////////////////////////////////////////

#include "MiddleEarthGraph.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////
//                              Public methods                               //
///////////////////////////////////////////////////////////////////////////////

// Graph Constuctor
// Parameters: nodesWithDistances - take a specificly formatted file's name and 
//                                  builds a graph with it
Graph::Graph(string nodesWithDistances) {
	locCounter = 0;
	string currentNode;
	ifstream inFile;
	inFile.open(nodesWithDistances.c_str());

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
		int d;
		string to;

		for (int i = 0; i < x; i++) {
			getline(inFile, to);
			string temp;
			getline(inFile, temp);
			d = stringToInt(temp.substr(0, temp.find(" ")));
			temp.erase(0, temp.find(" ") + 1);
			temp.erase(0, temp.find(" ") + 1);

			addEdge(from, to, d);
			addEdge(to, from, d);
		}
	}
}

void Graph::runACO(double alpha, double beta, double rho) {
	int x;

	this->alpha = alpha;
	this->beta = beta;
	this->rho = rho;

	antCycles();	
}

// Post-Condition: Builds a string of all the node data and prints
// Returns: printLocs - the constructed string of data
string Graph::toString() {
	string ans = "";


	for (int a = 0; a < TOTAL_ANTS; a++) {
		ans += "\nDistance: " + intToString(ants[a].pathSum) + "\nPath: ";
		for (int i = 0; i < ants[a].actualPath.size() - 1; i++)
			ans += locations[ants[a].actualPath[i]].locationName + ", ";
		ans += locations[ants[a].actualPath[ants[a].actualPath.size() - 1]].locationName;
		ans += "\n";
	}
	return ans;
}

///////////////////////////////////////////////////////////////////////////////
//                             Private methods                               //
///////////////////////////////////////////////////////////////////////////////

// Initialize ants PDM
void Graph::initializeAnts() {
	ants = new Ant[TOTAL_ANTS];
	int start = getNode("Blue Mountains");
	for (int i = 0; i < TOTAL_ANTS; i++) {
		ants[i].location = start;
		ants[i].path.push_back(start);
		ants[i].actualPath.push_back(start);
		ants[i].pathSum = 0;
	}
}

// cyle ants on map
void Graph::antCycles() {
	initializeAnts();
	while (!allAntsAtGoal())
		firstMove();
	for (int i = 1; i < CYCLES; i++) {
				
		addPheremones();
		decayPheremones();
		updateProbabilities();

		initializeAnts();

		while (!allAntsAtGoal())
			antsMove();		
	}
}

// Initializes first move of ants
void Graph::firstMove() {
	for (int ant = 0; ant < TOTAL_ANTS; ant++) {
		
		while (!antAtGoal(ant)) {			
			Edge edge;
			
			int size = locations[ants[ant].location].edgeCount;

			bool moved = false;

			for (int i = 0; i < size; i++) {
				edge = locations[ants[ant].location].edges[i];
				if (edge.to.compare("Iron Hills") == 0) {
					updateAnt(ant, getNode(edge.to), ants[ant].pathSum + edge.distance);
					moved = true;
					break;
				}
			}

			if (!moved) {
				edge = locations[ants[ant].location].edges[rand() % size];
				updateAnt(ant, getNode(edge.to), ants[ant].pathSum + edge.distance);
			}
		}
	}
}

// move ants
void Graph::antsMove() {
	for (int i = 0; i < TOTAL_ANTS; i++)
		while (!antAtGoal(i))
			antMove(i);
}

// move ant based on integer location 
void Graph::antMove(int ant) {	
	Edge edge = getNextEdge(ant);
	
	// the new values for the ant that moved
	int newLocation = getNode(edge.to);
	int newPathSum = ants[ant].pathSum + edge.distance;

	// updates that ants new location and path and path sum
	updateAnt(ant, newLocation, newPathSum);
}

// returns the next edge for a given ant
Graph::Edge Graph::getNextEdge(int ant) {
	
	priority_queue<Edge, vector<Edge>, CompareEdge> edges;
	int size = locations[ants[ant].location].edgeCount;

	for (int i = 0; i < size; i++) {
		if (locations[ants[ant].location].edges[i].to.compare("Iron Hills") == 0) {
			return locations[ants[ant].location].edges[i];
		}
		edges.push(locations[ants[ant].location].edges[i]);
	}

	Edge edge;
	
	bool empty = false;

	do {
		if (edges.empty())
			break;

		edge = edges.top();
		edges.pop();
		
		if (edge.to.compare("Iron Hills") == 0)
			return edge;
			
		if (!isDeadEnd(ant, edge) && !containsNode(ant, getNode(edge.to)))				
			if (edges.empty() || ((double)rand() / (RAND_MAX)) <= edge.probability)
				return edge;
			
		} while (isDeadEnd(ant, edge) || containsNode(ant, getNode(edge.to)));
		
		if (isDeadEnd(ant, edge)) {
			Edge e;
			e.distance = getDistance(ants[ant].location, ants[ant].path[ants[ant].path.size() - 2]);
			e.pheremone = 0;
			e.probability = 0;
			e.to = locations[ants[ant].path[ants[ant].path.size() - 2]].locationName;
			return e;
		}

	return edge;
}

// returns the distance
int Graph::getDistance(int curr, int next) {
	int size = locations[curr].edgeCount;
	for (int i = 0; i < size; i++) {
		if (getNode(locations[curr].edges[i].to) == next)
			return locations[curr].edges[i].distance;
	}
}

// predicates if a given ant on a given edge is at a dead end
bool Graph::isDeadEnd(int ant, Edge edge) {
	int size = locations[getNode(edge.to)].edgeCount;
	int loc = getNode(edge.to);
	for (int i = 0; i < size; i++) {		
		int check = getNode(locations[loc].edges[i].to);
		if (!containsNode(ant, check))
			return false;
	}
	return true;
}

// udate the given ant with new location and path sum
void Graph::updateAnt(int ant, int newLocation, int newPathSum) {
	ants[ant].pathSum = newPathSum;
	ants[ant].location = newLocation;	
	if (!containsNode(ant, newLocation))
		ants[ant].path.push_back(newLocation);
	ants[ant].actualPath.push_back(newLocation);
}

// increases pheromone level accross the map
void Graph::addPheremones() {
	// Update pheremones then decay for each ants path
	for (int ant = 0; ant < TOTAL_ANTS; ant++) {

		// get the values for updating a path
		int pathSize = ants[ant].path.size();
		int pathSum = ants[ant].pathSum;

		// place pheremone on edges in both directions
		for (int i = 0; i < pathSize - 1; i++) {

			// the current node and the next node in the path
			int current = ants[ant].path[i];
			int next = ants[ant].path[i + 1];

			// place pheremone on both edges since i designed
			// my graph like a noob
			addPheremone(current, next, pathSum);
			addPheremone(next, current, pathSum);
		}
	}
}

// increases pheromone for the path
void Graph::addPheremone(int current, int next, int pathSum) {
	int edgeSize = locations[current].edgeCount;
	for (int i = 0; i < edgeSize; i++) {
		int to = getNode(locations[current].edges[i].to);
		if (to == next)
			locations[current].edges[i].pheremone += getPheremone(pathSum);
	}
}

// decays pheromone on the map
void Graph::decayPheremones() {
	for (int i = 0; i < locCounter; i++)
		decayPheremone(i, locations[i].edgeCount);
}

// decay pheromone for the path
void Graph::decayPheremone(int node, int edgeCount) {
	if (rho >= 1)
		rho = .95;
	for (int i = 0; i < edgeCount; i++)
		locations[node].edges[i].pheremone *= ((double)1.0 - rho);
}

// calculates the new probability
void Graph::updateProbabilities() {
	for (int i = 0; i < locCounter; i++)
		updateProbability(i, locations[i].edgeCount);
}

// updates probability for a given node
void Graph::updateProbability(int node, int edgeCount) {
	
	// get the sum of all the edge weights for the denominator
	double denominator = getSumOfWeights(node, edgeCount);
	for (int i = 0; i < edgeCount; i++) {
		// get the pherimone weighter with alpha
		double weightedPheremone = pow(locations[node].edges[i].pheremone, alpha);

		// get the distance weighted with beta
		double weightedDistance = pow((1.0 / (double)locations[node].edges[i].distance), beta);

		// caculate the numerator
		double numerator = weightedPheremone * weightedDistance;

		// update the probability
		if (denominator != 0)
			locations[node].edges[i].probability = (numerator / denominator);
	}
}

// returns the sum of the weights
double Graph::getSumOfWeights(int node, int edgeCount) {
	double weightedSum = 0.0;
	for (int i = 0; i < edgeCount; i++) {
		double weightedPheremone = pow(locations[node].edges[i].pheremone, alpha);
		double weightedDistance = pow((1.0 / (double)locations[node].edges[i].distance), beta);
		weightedSum = weightedSum + (weightedPheremone * weightedDistance);
	}
	return weightedSum;
}

// returns the phermone
double Graph::getPheremone(int pathSum) {
	return TOTAL_PHEREMONE / (double)pathSum;
}

// predicates if an ants path contains a node(city)
bool Graph::containsNode(int ant, int to) {
	for (int i = 0; i < ants[ant].path.size(); i++)
		if (to == ants[ant].path[i])
			return true;
	return false;
}

// predicates if all ants in PDM are at goal node
bool Graph::allAntsAtGoal() {
	for (int ant = 0; ant < TOTAL_ANTS; ant++)
		if (!antAtGoal(ant))
			return false;
	return true;
}

// predicates if given ant i is at goal node
bool Graph::antAtGoal(int ant) {
	return (locations[ants[ant].location].locationName.compare("Iron Hills") == 0);
}

// Parameters : from - the current location
//                to - the location the edge goes to
//                 d - the distance of the edge
// Post-Condition: addes an edge to the from node
void Graph::addEdge(string from, string to, int d) {
	int temp = getNode(from);

	// I assume you want to do this for all the edges? and not just the edge count
	locations[temp].edges[locations[temp].edgeCount].to = to;
	locations[temp].edges[locations[temp].edgeCount].distance = d;
	locations[temp].edges[locations[temp].edgeCount].pheremone = 0.1;	
	locations[temp].edges[locations[temp].edgeCount].probability = 0.0;
	locations[temp].edgeCount++;
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

