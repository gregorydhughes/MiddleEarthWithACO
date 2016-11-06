//	Author:		Garry Griggs and Gregory Hughes
//	Date:		October 24th 2016
//	Project:	A* algorithm
//	File:		IronHillsClient.cpp
//	Purpose:	Client uses the graph to find the shortest path from any user 
//					given node

///////////////////////////////////////////////////////////////////////////////

#include "MiddleEarthGraph.h"
#include "Heuristics.h"
#include "ExceptionClass.h"

using namespace std;

// this is a file of all the nodes with distance from goals
const string NODES_WITH_DISTANCES_TO_IRON_HILLS = "distancetable.txt";

// this is a file with all the local connections to other nodes
const string CONNECTIONS_BETWEEN_NODES = "connectiontable.txt";

// File to output results to
const string OUTPUT_FILE = "file.out";

// Displays a prompt to user and returns a response (string) to caller
string getStartingCity();

// prompts a user for the heuristic to use
double getABR(string in);

int main() {

	// Let's try something else
	try {
		Graph middleEarth = Graph(NODES_WITH_DISTANCES_TO_IRON_HILLS);
		middleEarth.addEdges(CONNECTIONS_BETWEEN_NODES);

		int alpha, beta, rho;
		
		alpha = getABR("alpha");
		beta = getABR("beta");
		rho = getABR("rho");

		// begin A*
		middleEarth.runACO(alpha, beta, rho);

		// Stream for output
		ofstream fout;
		fout.open(OUTPUT_FILE.c_str());

		// Print results!
		fout << middleEarth.toString();
		cout << middleEarth.toString();

		// Close the stream
		fout.close();

	}
	// Something happened
	catch (...) {
		string quote = "It's a dangerous business, Frodo, going out your door.";
		quote += "You step onto the road, and if you don't keep your feet, ";
		quote += "there's no knowing where you might be swept off to.";
		cout << quote << endl << endl;

		cout << "But some exception was thrown, and we're dead. "
			<< "We failed to make it, to sell our wares. " << endl;
	}

	// End program
	return 0;
}// End main

string getStartingCity() {

	string question = "Where would you like to begin your quest?\t";

	// Ask your question
	cout << question;

	// Variable to hold response
	string ans;

	// Get response
	getline(cin, ans);

	// Return response to caller
	return ans;
}// End getStartingCity

double getABR(string in) {
	string question = "What is the value you want for ";
	question += in + "?\t";

	// Answer a question
	cout << question;

	// Variable for answer
	double input;

	// Get response
	cin >> input;

	// return response
	return input;
}// End getHeuristic