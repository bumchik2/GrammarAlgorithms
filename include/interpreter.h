#pragma once

#include "pascal_variable.h"
#include "syntax_tree.h"

#include <unordered_map>
#include <sstream>
#include <ostream>

using std::unordered_map;
using std::ostream;

class Interpreter {
public:
	void interpret(const SyntaxTree& syntax_tree, const Grammar& grammar, ostream& out = cout);
	void printVariables(ostream& out = cout) const;
private:
	shared_ptr<PascalVariable> getExpressionValue(const SyntaxTree::Node* const, const Grammar&);
	void interpret(const SyntaxTree::Node* const node, const Grammar& grammar, ostream& out = cout);
	void interpret(const SyntaxTree::Node* const node, const Grammar& grammar, int, ostream& out = cout);
	unordered_map<string, shared_ptr<PascalVariable>> variables;
};
