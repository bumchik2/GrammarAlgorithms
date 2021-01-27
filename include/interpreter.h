#pragma once

#include "pascal_variable.h"
#include "syntax_tree.h"

#include <unordered_map>

using std::unordered_map;

class Interpreter {
public:
	void interpret(const SyntaxTree& syntax_tree, const Grammar& grammar);
	void printVariables() const;
private:
	shared_ptr<PascalVariable> getExpressionValue(const SyntaxTree::Node* const, const Grammar&);
	void interpret(const SyntaxTree::Node* const node, const Grammar& grammar);
	void interpret(const SyntaxTree::Node* const node, const Grammar& grammar, int);
	unordered_map<string, shared_ptr<PascalVariable>> variables;
};
