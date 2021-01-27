#pragma once

#include "grammar.h"

#include <vector>
#include <iostream>

using std::vector;
using std::ostream;

class SyntaxTree {
public:
	struct Node {
		~Node();
		string symbol;
		int rule_number = -1;
		vector<Node*> nodes;
	};
	SyntaxTree() = default;
	SyntaxTree(const SyntaxTree&);
	~SyntaxTree();
	SyntaxTree& operator = (const SyntaxTree&);
	Node* root = nullptr;
};

SyntaxTree getSyntaxTree(const Grammar& grammar, const string& word);
string getName(const SyntaxTree::Node* const node);
vector<string> getVariables(const SyntaxTree::Node* const node);

SyntaxTree::Node* copy(const SyntaxTree::Node* const);

ostream& operator << (ostream& os, const SyntaxTree::Node* const);
ostream& operator << (ostream& os, const SyntaxTree&);
