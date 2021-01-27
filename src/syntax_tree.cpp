#include "syntax_tree.h"
#include "grammar.h"
#include "earley.h"

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

SyntaxTree getSyntaxTree(const Grammar& grammar, const string& word) {
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
