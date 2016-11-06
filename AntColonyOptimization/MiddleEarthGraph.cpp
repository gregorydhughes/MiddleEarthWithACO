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

	srand(time(NULL));
}

void Graph::runACO(double alpha, double beta, double rho) {
	this->alpha = alpha;
	this->beta = beta;
	this->rho = rho;

	void initializeAnts();
	void antCycles();
	
}

void Graph::antCycles() {
	for (int i = 0; i < CYCLES; i++) {
		while (!allAntsAtGoal())
			antsMove();
		addPheremones();
		decayPheremones();
		updateProbabilies();
	}
}

void Graph::updateProbabilies() {
	for (int i = 0; i < 25; i++)
		updateProbability(i, locations[i].edgeCount);
}

void Graph::updateProbability(int node, int edgeCount) {

	// get the sum of all the edge weights for the denominator
	int denominator = getSumOfWeights(node, edgeCount);

	for (int i = 0; i < edgeCount; i++) {
		// get the pherimone weighter with alpha
		double weightedPheremone = pow(locations[node].edges[i].pheremone, alpha);

		// get the distance weighted with beta
		double weightedDistance = pow((1.0 / (double)locations[node].edges[i].distance), beta);

		// caculate the numerator
		double numerator = weightedPheremone * weightedDistance;

		// update the probability
		locations[node].edges[i].probability = (numerator / denominator);
	}
}

double Graph::getSumOfWeights(int node, int edgeCount) {
	double weightedSum = 0;
	for (int i = 0; i < edgeCount; i++) {
		double weightedPheremone = pow(locations[node].edges[i].pheremone, alpha);
		double weightedDistance = pow((1.0 / (double)locations[node].edges[i].distance), beta);
		double weightedSum = weightedSum + (weightedPheremone * weightedDistance);
	}
	return weightedSum;
}

void Graph::decayPheremones() {
	for (int i = 0; i < 25; i++)
		decayPheremone(i, locations[i].edgeCount);
}

void Graph::decayPheremone(int node, int edgeCount) {
	for (int i = 0; i < edgeCount; i++)
		locations[node].edges[i].pheremone *= (1 - rho);
}

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

void Graph::addPheremone(int current, int next, int pathSum) {
	int edgeSize = locations[current].edgeCount;
	for (int i = 0; i < edgeSize; i++) {
		int to = getNode(locations[current].edges[i].to);
		if (to == next)
			locations[current].edges[i].pheremone += getPheremone(pathSum);
	}
}

double Graph::getPheremone(int pathSum) {
	return TOTAL_PHEREMONE / pathSum;
}

void Graph::antsMove() {
	for (int i = 0; i < TOTAL_ANTS; i++)
		if (!antAtGoal)
			antMove(i);
}

bool Graph::antMove(int ant) {
	// max pheremone
	int maxProbability = 0;

	// all the edges equal to max
	vector<Edge> maxEdges;

	// the list size of the edges list from the node
	int edges = locations[ants[ant].location].edgeCount;

	// gets the max probability
	for (int i = 0; i < edges; i++) {
		int probability = locations[ants[ant].location].edges[i].probability;
		int toLocation = getNode(locations[ants[ant].location].edges[i].to);
		if (!containNode(ant, toLocation) && probability > maxProbability)
			maxProbability = probability;
	}
	
	// stores all the edges equal to max probability into maxEdges
	for (int i = 0; i < edges; i++) {
		int probability = locations[ants[ant].location].edges[i].probability;
		int toLocation = getNode(locations[ants[ant].location].edges[i].to);
		if (!containNode(ant, toLocation) && probability == maxProbability)
			maxEdges.push_back(locations[ants[ant].location].edges[i]);
	}
	
	// grabs a random edge to travel
	Edge edge = maxEdges[rand() % maxEdges.size()];

	// the new values for the ant that moved
	int newLocation = getNode(edge.to);
	int newPathSum = ants[ant].pathSum + edge.distance;
	
	// updates that ants new location and path and path sum
	updateAnt(ant, newLocation, newPathSum);	
}

bool Graph::containNode(int ant, int to) {
	for (int i = 0; i < ants[ant].path.size(); i++)
		if (to == ants[ant].path[i])
			return true;
	return false;
}

bool Graph::updateAnt(int ant, int newLocation, int newPathSum) {
	ants[ant].pathSum = newPathSum;
	ants[ant].location = newLocation;
	ants[ant].path.push_back(newLocation);
}

bool Graph::allAntsAtGoal() {
	for (int i = 0; i < TOTAL_ANTS; i++)
		if (!antAtGoal(i))
			return false;
	return true;
}

bool Graph::antAtGoal(int i) {
	return (locations[ants[i].location].locationName.compare("Iron Hills") != 0);
}

void Graph::initializeAnts() {
	ants = new Ant[TOTAL_ANTS];
	int start = getNode("Blue Mountains");
	for (int i = 0; i < 10; i++) {
		ants[i].location = 0;
		ants[i].path.push_back(start);
		ants[i].pathSum = 0;
	}
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
	locations[temp].edges[locations[temp].edgeCount].pheremone = 0;
	locations[temp].edges[locations[temp].edgeCount].probability = 0;
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
	return "";
}