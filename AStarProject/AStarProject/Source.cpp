#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <functional>
#include <queue>

#define LEFT 'L'
#define RIGHT 'R'
#define UP 'U'
#define DOWN 'D'

using namespace std;

struct PuzzleState {
	int matrix[3][3];
	int XPos;
	int YPos;
	int manhattanDistance;

	bool operator==(const PuzzleState& otherState) const {
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

ostream& operator<<(ostream& out, const PuzzleState& state) {
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

struct cmpNodePtrs
{
	bool operator()(const Node* lhs, const Node* rhs) const
	{
		return lhs->totalCost > rhs->totalCost;
	}
};

void setState(PuzzleState& initialState, PuzzleState& goalState, string inputFile) {
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

PuzzleState MoveUp(const PuzzleState& currState) {
	PuzzleState nextState = currState;
	nextState.matrix[nextState.XPos][nextState.YPos] = nextState.matrix[nextState.XPos - 1][nextState.YPos]; //first set what value at 0 will be
	nextState.matrix[nextState.XPos - 1][nextState.YPos] = 0; //then set new 0
	nextState.XPos -= 1;
	return nextState;
}

PuzzleState& MoveDown(const PuzzleState& currState) {
	PuzzleState nextState = currState;
	nextState.matrix[nextState.XPos][nextState.YPos] = nextState.matrix[nextState.XPos + 1][nextState.YPos];
	nextState.matrix[nextState.XPos + 1][nextState.YPos] = 0;
	nextState.XPos += 1;
	return nextState;
}

PuzzleState& MoveLeft(const PuzzleState& currState) {
	PuzzleState nextState = currState;
	nextState.matrix[nextState.XPos][nextState.YPos] = nextState.matrix[nextState.XPos][nextState.YPos - 1];
	nextState.matrix[nextState.XPos][nextState.YPos - 1] = 0;
	nextState.YPos -= 1;
	return nextState;
}

PuzzleState& MoveRight(const PuzzleState& currState) {
	PuzzleState nextState = currState;
	nextState.matrix[nextState.XPos][nextState.YPos] = nextState.matrix[nextState.XPos][nextState.YPos + 1];
	nextState.matrix[nextState.XPos][nextState.YPos + 1] = 0;
	nextState.YPos += 1;
	return nextState;
}

bool checkDupe(PuzzleState& newState, vector<Node*> visited) {
	for (Node* nodeCheck : visited) {
		if (nodeCheck->state == newState) {
			return true;
		}
	}
	return false;
}

void calculateMD(PuzzleState& state, const PuzzleState& goalState) { //calculates manhattan distance and updates
	int MD = 0;
	int x, y, x2, y2; // (x2,y2) are goal coords, (x,y2) are state coords
	for (int i = 0; i < 9; ++i) {
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
		MD += (abs(x - x2) + abs(y - y2));
	}
	
	state.manhattanDistance = MD;

}

void checkValidandRun(Node& currNode, PuzzleState& goalState, priority_queue<Node*, vector<Node*>, cmpNodePtrs>& frontier, const vector<Node*>& visited) {
	if (currNode.state.YPos > 0) {
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
	if (currNode.state.YPos < 2) {
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
	if (currNode.state.XPos < 2) {
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
	if (currNode.state.XPos> 0) {
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

void printPath(Node* goalNode) {
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

	priority_queue<Node*, vector<Node*>, cmpNodePtrs> frontier;
	vector<Node*> visited;
	
	Node* initialNode = new Node(initialState, 0, NULL);
	frontier.push(initialNode);
	Node* nextNode;
	int totalStates = frontier.size() + visited.size();

	while (frontier.empty() == false) {
		nextNode = frontier.top();
		visited.push_back(nextNode);
		frontier.pop();
		if (nextNode->state == goalState) {
			printPath(nextNode);
			cout << endl;

			cout << "counter " << frontier.size() + visited.size() << endl;
			break;
		}

		checkValidandRun(*nextNode, goalState, frontier, visited);
	}

	getchar();
	
	
}