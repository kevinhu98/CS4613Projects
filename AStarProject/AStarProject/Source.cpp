#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <functional>
#include <queue>

using namespace std;

struct PuzzleState {
	int matrix[3][3]; // (x,y)
	int XPos; //Zero XPos
	int YPos; //Zero YPos
	int manhattanDistance;

	bool operator==(const PuzzleState& otherState) const { // Compares 2 puzzle states
		for (int i = 0; i < 3; ++i) {
			for (int j = 0; j < 3; ++j) {
				if (this->matrix[i][j] != otherState.matrix[i][j]) {
					return false;
				}
			}
		}
		return true;
	}
	friend ostream & operator << (ostream &out, const PuzzleState &state);
};

ostream& operator<<(ostream& out, const PuzzleState& state) { // output operator for puzzle state 
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			cout << state.matrix[i][j] << " ";
		}
		cout << endl;
	}
	return out;
}

struct Node {
	int depth;
	PuzzleState state;
	char direction;
	vector<Node*> children;
	Node* parent;
	int totalCost; // depth + manhattanDistance
	
	Node(PuzzleState state, char direction, int depth) : state(state), direction(direction), depth(depth) {}
};

struct cmpNodePtrs //used as parameter for implementing priority queue
{
	bool operator()(const Node* lhs, const Node* rhs) const
	{
		return lhs->totalCost > rhs->totalCost; //allows priority queue to be sorted backwards, smallest cost at the top
	}
};

void setState(PuzzleState& initialState, PuzzleState& goalState, string inputFile) { //reads the file and sets initial and goal states
	ifstream ifs(inputFile);
	string line;
	int column0, column1, column2;
	for (int i = 0; i < 3; i++) {
		ifs >> column0 >> column1 >> column2;
		initialState.matrix[i][0] = column0;
		initialState.matrix[i][1] = column1;
		initialState.matrix[i][2] = column2;
	}
	getline(ifs, line);
	for (int i = 0; i < 3; i++) {
		ifs >> column0 >> column1 >> column2;
		goalState.matrix[i][0] = column0;
		goalState.matrix[i][1] = column1;
		goalState.matrix[i][2] = column2;
	}
}

PuzzleState MoveUp(const PuzzleState& currState) { //creates new puzzle state based on where zero will go 
	PuzzleState nextState = currState; //create copy of input state
	nextState.matrix[nextState.XPos][nextState.YPos] = nextState.matrix[nextState.XPos - 1][nextState.YPos]; //first set what value at 0 will be
	nextState.matrix[nextState.XPos - 1][nextState.YPos] = 0; //then set new 0
	nextState.XPos -= 1; //change zero coords
	return nextState;
}

PuzzleState& MoveDown(const PuzzleState& currState) { //creates new puzzle state based on where zero will go 
	PuzzleState nextState = currState; //create copy of input state
	nextState.matrix[nextState.XPos][nextState.YPos] = nextState.matrix[nextState.XPos + 1][nextState.YPos]; //first set what value at 0 will be
	nextState.matrix[nextState.XPos + 1][nextState.YPos] = 0; //then set new 0
	nextState.XPos += 1; //change zero coords
	return nextState;
}

PuzzleState& MoveLeft(const PuzzleState& currState) { //creates new puzzle state based on where zero will go 
	PuzzleState nextState = currState; //create copy of input state
	nextState.matrix[nextState.XPos][nextState.YPos] = nextState.matrix[nextState.XPos][nextState.YPos - 1]; //first set what value at 0 will be
	nextState.matrix[nextState.XPos][nextState.YPos - 1] = 0; //then set new 0
	nextState.YPos -= 1; //change zero coords
	return nextState;
}
 
PuzzleState& MoveRight(const PuzzleState& currState) { //creates new puzzle state based on where zero will go 
	PuzzleState nextState = currState; //create copy of input state
	nextState.matrix[nextState.XPos][nextState.YPos] = nextState.matrix[nextState.XPos][nextState.YPos + 1]; //first set what value at 0 will be
	nextState.matrix[nextState.XPos][nextState.YPos + 1] = 0; //then set new 0
	nextState.YPos += 1; //change zero coords
	return nextState;
}

bool checkDupe(PuzzleState& newState, vector<Node*> visited) { //checks if a state is already in visited array
	for (Node* nodeCheck : visited) {
		if (nodeCheck->state == newState) {
			return true; //if state has been visited, return true
		}
	}
	return false; //if state has not been visited, return false
}

void calculateMD(PuzzleState& state, const PuzzleState& goalState) { //calculates manhattan distance and updates
	int MD = 0;
	int x, y, x2, y2; // (x2,y2) are goal coords, (x,y2) are state coords
	for (int i = 0; i < 9; ++i) { //iterates 9 times, to calculate manhattan distance of each value 0-8
		for (int j = 0; j < 3; ++j) {
			for (int k = 0; k < 3; ++k) {
				if (state.matrix[j][k] == i) {
					x = j;
					y = k;
				}
				if (goalState.matrix[j][k] == i) {
					x2 = j;
					y2 = k;
				}
			}
		}
		MD += (abs(x - x2) + abs(y - y2)); //repeatedly increases manhattan distance
	}
	state.manhattanDistance = MD; //sets manhattan distance
}

void expand(Node& currNode, PuzzleState& goalState, priority_queue<Node*, vector<Node*>, cmpNodePtrs>& frontier, const vector<Node*>& visited) { //expands possible routes for node
	if (currNode.state.YPos > 0) { //checks if zero can move left
		PuzzleState newState = MoveLeft(currNode.state); 
		if (checkDupe(newState, visited) == false) { 
			calculateMD(newState, goalState); 
			Node* childNode = new Node(newState, 'L', currNode.depth + 1);
			childNode->totalCost = childNode->depth + childNode->state.manhattanDistance;
			childNode->parent = &currNode;
			currNode.children.push_back(childNode);
			frontier.push(childNode);
		}
	}
	if (currNode.state.YPos < 2) {//checks if zero can move right
		PuzzleState newState = MoveRight(currNode.state);
		if (checkDupe(newState, visited) == false) {
			calculateMD(newState, goalState);
			Node* childNode = new Node(newState, 'R', currNode.depth + 1);
			childNode->totalCost = childNode->depth + childNode->state.manhattanDistance;
			childNode->parent = &currNode;
			currNode.children.push_back(childNode);
			frontier.push(childNode);
		}
	}
	if (currNode.state.XPos < 2) { //checks if zero can move down
		PuzzleState newState = MoveDown(currNode.state);
		if (checkDupe(newState, visited) == false) {
			calculateMD(newState, goalState);
			Node* childNode = new Node(newState, 'D', currNode.depth + 1);
			childNode->totalCost = childNode->depth + childNode->state.manhattanDistance;
			childNode->parent = &currNode;
			currNode.children.push_back(childNode);
			frontier.push(childNode);
		}
	}
	if (currNode.state.XPos> 0) { //checks if zero can move up
		PuzzleState newState = MoveUp(currNode.state);
		if (checkDupe(newState, visited) == false) {
			calculateMD(newState, goalState);
			Node* childNode = new Node(newState, 'U', currNode.depth + 1);
			childNode->totalCost = childNode->depth + childNode->state.manhattanDistance;
			childNode->parent = &currNode;
			currNode.children.push_back(childNode);
			frontier.push(childNode);
		}
	}
}

void setZero(PuzzleState& initialState) { //finds and sets initial zero location
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			if (initialState.matrix[i][j] == 0) {
				initialState.XPos = i;
				initialState.YPos = j;
			}
		}
	}
}

void printPath(Node* goalNode) { //recursively prints out path to reach goal
	if (goalNode->direction == NULL) {
		return;
	}
	else {
		printPath(goalNode->parent);
		cout << goalNode->direction << " ";
	}
}

int main() {
	PuzzleState initialState, goalState;
	setState(initialState, goalState, "Input2.txt"); // reads file and initializes states
	setZero(initialState); //locates and sets zero of initial
	calculateMD(initialState, goalState); //calculates and sets initial MD

	priority_queue<Node*, vector<Node*>, cmpNodePtrs> frontier; //nodes that are going to be visited
	vector<Node*> visited; //nodes that have been visited
	
	Node* initialNode = new Node(initialState, 0, NULL);
	frontier.push(initialNode);
	Node* nextNode;
	int totalStates = frontier.size() + visited.size(); //total number of states 
	bool solved = false; //is solved flag

	while (frontier.empty() == false) { //must visit everything in frontier
		nextNode = frontier.top();
		visited.push_back(nextNode);
		frontier.pop();
		if (nextNode->state == goalState) { //if it reaches goal, break
			printPath(nextNode);
			cout << endl;

			cout << "counter " << frontier.size() + visited.size() << endl;
			bool solved = true; //solution is found
			break;
		}
		expand(*nextNode, goalState, frontier, visited);
	}

	if (!solved) {
		cout << " no solution " << endl;
	}
	getchar();
}