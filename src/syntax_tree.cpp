#include "syntax_tree.h"
#include "grammar.h"
#include "earley.h"
#include "lr_algorithm.h"

#include <string>
#include <iostream>
#include <vector>

using std::string;
using std::cin;
using std::cout;
using std::endl;
using std::vector;
using std::ostream;

SyntaxTree::Node::~Node() {
	for (unsigned i = 0; i < nodes.size(); ++i) {
		delete nodes[i];
	}
}

SyntaxTree::~SyntaxTree() {
	delete root;
}

SyntaxTree::Node* copy(const SyntaxTree::Node* const node) {
	if (node == nullptr)
		return nullptr;
	SyntaxTree::Node* new_node = new SyntaxTree::Node();
	new_node->rule_number = node->rule_number;
	new_node->symbol = node->symbol;
	new_node->nodes.resize(node->nodes.size());
	for (unsigned i = 0; i < new_node->nodes.size(); ++i) {
		new_node->nodes[i] = copy(node->nodes[i]);
	}
	return new_node;
}

SyntaxTree::SyntaxTree(const SyntaxTree& syntax_tree) {
	root = copy(syntax_tree.root);
}

SyntaxTree& SyntaxTree::operator = (const SyntaxTree& syntax_tree) {
	root = copy(syntax_tree.root);
	return *this;
}

bool isRecognized(const Grammar& grammar, const string& word) {
	// return LRAlgorithm(grammar).isRecognized(word);
	return EarleyAlgorithm().isRecognized(grammar, word);
}

SyntaxTree getSyntaxTree(const Grammar& grammar, const string& word) {
	// return LRAlgorithm(grammar).getSyntaxTree(word);
	return EarleyAlgorithm().getSyntaxTree(grammar, word);
}

ostream& operator << (ostream& os, const SyntaxTree::Node* const node) {
	os << node->symbol << endl;
	for (unsigned i = 0; i < node->nodes.size(); ++i) {
		os << "going down" << endl << node->nodes[i] << endl << "going up" << endl;
	}
	return os;
}

ostream& operator << (ostream& os, const SyntaxTree& syntax_tree) {
	os << syntax_tree.root;
	return os;
}

string getName(const SyntaxTree::Node* const node) {
	if (node->nodes.size() == 0) {
		return node->symbol;
	}
	string result;
	for (unsigned i = 0; i < node->nodes.size(); ++i) {
		result += getName(node->nodes[i]);
	}
	return result;
}

bool areEqual(const SyntaxTree::Node* const node1, const SyntaxTree::Node* const node2) {
	if (node1->symbol != node2->symbol || node1->rule_number != node2->rule_number ||
				node1->nodes.size() != node2->nodes.size()) {
		return false;
	}
	for (unsigned i = 0; i < node1->nodes.size(); ++i) {
		if (!areEqual(node1->nodes[i], node2->nodes[i])) {
			return false;
		}
	}
	return true;
}

bool operator == (const SyntaxTree& syntax_tree1, const SyntaxTree& syntax_tree2) {
	return areEqual(syntax_tree1.root, syntax_tree2.root);
}
