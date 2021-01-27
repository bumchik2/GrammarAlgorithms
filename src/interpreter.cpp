#include "interpreter.h"
#include "syntax_tree.h"
#include "pascal_variable.h"

#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>

using std::ostringstream;
using std::vector;
using std::string;
using std::unordered_set;

const vector<Rule> interpret_rules = {
	{"Declaration", {"Variables", "Separator", ":", "Separator", "Type", ";", "Separator"}},
	{"Instruction", {"Variable", "Separator", ":", "=",
			"Separator", "Expression", "Separator", ";", "Separator"}},
	{"Instruction", {"Writeln", "Separator", "(", "Separator", "Expression", "Separator", ")", ";", "Separator"}},
	{"Instruction", {"If", "Separator", "Expression", "Separator", "Then",
			"Separator", "Instruction", "Separator", "ElsePart"}},
	{"Instruction",  {"While", "Separator", "Expression", "Separator", "Do", "Separator", "Instruction"}},
	{"Instruction", {"For", "Separator", "Variable", "Separator", ":", "=", "Separator", "Expression", "Separator",
			"To", "Separator", "Expression", "Separator", "Do", "Separator", "Instruction"}},
	{"Instruction", {"Variable", "Separator", "[", "Separator", "Expression", "Separator",
			"]", "Separator", ":", "=", "Separator", "Expression", "Separator", ";", "Separator"}},
	{"ElsePart", {"Else", "Separator", "Instruction", "Separator"}},
	{"Instruction", {"Write", "Separator", "(", "Separator", "Expression", "Separator", ")", ";", "Separator"}},
};

const vector<Rule> expression_rules = {
	{"Expression", {"LogicalExpression"}},
	{"MulExpression", {"Number"}},
	{"MulExpression", {"MulExpression", "Separator", "*", "Separator", "MulExpression"}},
	{"MulExpression", {"MulExpression", "Separator", "/", "Separator", "MulExpression"}},
	{"ArithmeticExpression", {"ArithmeticExpression", "Separator", "+", "Separator", "ArithmeticExpression"}},
	{"ArithmeticExpression", {"ArithmeticExpression", "Separator", "-", "Separator", "ArithmeticExpression"}},
	{"MulExpression", {"(", "Separator", "Expression", "Separator", ")"}},
	{"MulExpression", {"Variable"}},
	{"MulExpression", {"Separator", "-", "Separator", "MulExpression"}},
	{"LogicalBit", {"ArithmeticExpression", "Separator", "<", "Separator", "ArithmeticExpression"}},
	{"LogicalBit", {"ArithmeticExpression", "Separator", ">", "Separator", "ArithmeticExpression"}},
	{"LogicalBit", {"ArithmeticExpression", "Separator", "=", "Separator", "ArithmeticExpression"}},
	{"LogicalBit", {"ArithmeticExpression", "Separator", "<", ">", "Separator", "ArithmeticExpression"}},
	{"Expression", {"PackedExpression"}},
	{"PackedExpression", {"Variable"}},
	{"PackedExpression", {"(", "Separator", "ArithmeticExpression", "Separator", ")"}},
	{"MulExpression", {"PackedExpression", "Separator", "[", "Separator", "Expression", "Separator", "]", "Separator"}},
	{"MulExpression", {"'", "StringObject", "'"}},
	{"Expression", {"LogicalExpression"}},
	{"ArithmeticExpression", {"MulExpression"}},
	{"MulExpression", {"False"}},
	{"MulExpression", {"True"}},
	{"MulExpression", {"FloatingPoint"}},
	{"LogicalBit", {"ArithmeticExpression"}},
	{"LogicalExpression", {"LogicalBit"}},
	{"LogicalBit", {"(", "Separator", "LogicalExpression", "Separator", ")"}},
	{"LogicalExpression", {"LogicalBit", "Separator", "Or", "Separator", "LogicalBit"}},
	{"LogicalBit", {"LogicalBit", "Separator", "And", "Separator", "LogicalBit"}},
	{"LogicalBit", {"Not", "Separator", "LogicalBit"}}
};

vector<string> getVariables(const SyntaxTree::Node* const node, const Grammar& grammar) {
	if (grammar.rules[node->rule_number] == Rule{
			"Variables", {"Variable", "Separator"}}) {
		return {getName(node->nodes[0])};
	} else if (grammar.rules[node->rule_number] == Rule{"Variables",
			{"Variable", "Separator", ",", "Separator", "Variables"}}) {
		vector<string> answer = getVariables(node->nodes[4], grammar);
		answer.push_back(getName(node->nodes[0]));
		return answer;
	} else {
		throw runtime_error("unexpected error in getVariables");
	}
}

PascalType getType(const SyntaxTree::Node* const node) {
	return getType(getName(node));
}

void Interpreter::printVariables() const {
	bool first = true;
	cout << "Variables: ";
	for (const auto& variable : variables) {
		if (!first)
			cout << ", ";
		first = false;
		cout << "(" << getTypeName(variable.second->getType()) << " " << variable.first << " = ";
		variable.second->print();
		cout << ")";
	}
}

void Interpreter::interpret(const SyntaxTree& syntax_tree, const Grammar& grammar) {
	interpret(syntax_tree.root, grammar);
}

void Interpreter::interpret(const SyntaxTree::Node* const node, const Grammar& grammar) {
	if (node->symbol == "Separator" || node->rule_number == -1) {
	}

	if (find(interpret_rules.begin(), interpret_rules.end(),
			grammar.rules[node->rule_number]) == interpret_rules.end()) {
		for (unsigned i = 0; i < node->nodes.size(); ++i) {
			interpret(node->nodes[i], grammar);
		}
		return;
	}

	int special_rule_number = find(interpret_rules.begin(), interpret_rules.end(),
				grammar.rules[node->rule_number]) - interpret_rules.begin();
	interpret(node, grammar, special_rule_number);
}

shared_ptr<PascalVariable> Interpreter::getExpressionValue(const SyntaxTree::Node* const node, const Grammar& grammar) {
	if (find(expression_rules.begin(), expression_rules.end(),
			grammar.rules[node->rule_number]) == expression_rules.end()) {
		throw runtime_error("failed to calculate expression value");
	}
	int expression_rule_number = find(expression_rules.begin(), expression_rules.end(),
			grammar.rules[node->rule_number]) - expression_rules.begin();
	switch(expression_rule_number) {
	case 0: { // ""LogicalExpression""
		return getExpressionValue(node->nodes[0], grammar);
	} case 1: { // "Number"
		string number_str = getName(node->nodes[0]);
		return make_shared<PascalInteger>(stoi(number_str));
	} case 2: { // "MulExpression", "Separator", "*", "Separator", "MulExpression"
		return multiply(getExpressionValue(node->nodes[0], grammar), getExpressionValue(node->nodes[4], grammar));
		break;
	} case 3: { // "MulExpression", "Separator", "/", "Separator", "MulExpression"
		return divide(getExpressionValue(node->nodes[0], grammar), getExpressionValue(node->nodes[4], grammar));
		break;
	} case 4: { // "ArithmeticExpression", "Separator", "+", "Separator", "ArithmeticExpression"
		return sum(getExpressionValue(node->nodes[0], grammar), getExpressionValue(node->nodes[4], grammar));
	} case 5: { // "ArithmeticExpression", "Separator", "-", "Separator", "ArithmeticExpression"
		return diff(getExpressionValue(node->nodes[0], grammar), getExpressionValue(node->nodes[4], grammar));
	} case 6: { // "(", "Separator", "Expression", "Separator", ")"
		return getExpressionValue(node->nodes[2], grammar);
	} case 7: { // "Variable"
		return variables.at(getName(node->nodes[0]));
	} case 8: { // "Separator", "-", "Separator", "MulExpression"
		return negative(getExpressionValue(node->nodes[3], grammar));
	} case 9: { // "ArithmeticExpression", "Separator", "<", "Separator", "ArithmeticExpression"
		return isLess(getExpressionValue(node->nodes[0], grammar), getExpressionValue(node->nodes[4], grammar));
	} case 10: { // "ArithmeticExpression", "Separator", ">", "Separator", "ArithmeticExpression"
		return isGreater(getExpressionValue(node->nodes[0], grammar), getExpressionValue(node->nodes[4], grammar));
	} case 11: { // "ArithmeticExpression", "Separator", "=", "Separator", "ArithmeticExpression"
		return areEqual(getExpressionValue(node->nodes[0], grammar), getExpressionValue(node->nodes[4], grammar));
	} case 12: { // "ArithmeticExpression", "Separator", "<", ">", "Separator", "ArithmeticExpression"
		return areNotEqual(getExpressionValue(node->nodes[0], grammar), getExpressionValue(node->nodes[5], grammar));
	} case 13: { // "PackedExpression"
		return getExpressionValue(node->nodes[0], grammar);
	} case 14: { // "Variable"
		return variables.at(getName(node->nodes[0]));
	} case 15: { // "(", "Separator", "ArithmeticExpression", "Separator", ")"
		return getExpressionValue(node->nodes[2], grammar);
	} case 16: { // "PackedExpression", "Separator", "[", "Separator", "Expression", "Separator", "]", "Separator"
		shared_ptr<PascalVariable> str = getExpressionValue(node->nodes[0], grammar);
		if (str->getType() != String) {
			throw runtime_error("bad [] operand 1");
		}
		shared_ptr<PascalVariable> index = getExpressionValue(node->nodes[4], grammar);
		if (index->getType() != Integer) {
			throw runtime_error("bad [] operand 2");
		}
		return make_shared<PascalChar>((*reinterpret_cast<string*>(str->data))[*reinterpret_cast<int*>(index->data)]);
	} case 17: { // "'", "StringObject", "'"
		return make_shared<PascalString>(getName(node->nodes[1]));
	} case 18: { // "LogicalExpression"
		return getExpressionValue(node->nodes[0], grammar);
	} case 19: { // "MulExpression"
		return getExpressionValue(node->nodes[0], grammar);
	} case 20: { // "False"
		return make_shared<PascalBoolean>(false);
	} case 21: { // "True"
		return make_shared<PascalBoolean>(true);
	} case 22: { // "FloatingPoint"
		string str = getName(node->nodes[0]);
		return make_shared<PascalReal>(stof(str));
	} case 23: { // "ArithmeticExpression"
		return getExpressionValue(node->nodes[0], grammar);
	} case 24: { // "LogicalBit"
		return getExpressionValue(node->nodes[0], grammar);
	} case 25: { // "(", "Separator", "LogicalExpression", "Separator", ")"
		return getExpressionValue(node->nodes[2], grammar);
	} case 26: { // "LogicalBit", "Separator", "Or", "Separator", "LogicalBit"
		return make_shared<PascalBoolean>(isTrue(getExpressionValue(node->nodes[0], grammar)) ||
				isTrue(getExpressionValue(node->nodes[4], grammar)));
	} case 27: { // "LogicalBit", {"LogicalBit", "Separator", "And", "Separator", "LogicalBit"
		return make_shared<PascalBoolean>(isTrue(getExpressionValue(node->nodes[0], grammar)) &&
				isTrue(getExpressionValue(node->nodes[4], grammar)));
	} case 28: { // "Not", "Separator", "LogicalBit"
		return make_shared<PascalBoolean>(!isTrue(getExpressionValue(node->nodes[2], grammar)));
	} default: {
		throw runtime_error("failed to calculate expression value");
	}
	}
}

void Interpreter::interpret(const SyntaxTree::Node* const node,
		const Grammar& grammar, int interpret_rule_number) {
	switch (interpret_rule_number) {
	case 0: {
		vector<string> new_variables = getVariables(node->nodes[0], grammar);
		PascalType type = getType(node->nodes[4]);
		for (unsigned i = 0; i < new_variables.size(); ++i) {
			variables[new_variables[i]] = initializePascalVariable(type);
		}
		break;
	} case 1: { // "Instruction", {"Variable", "Separator", ":", "=",
			    // "Separator", "Expression", "Separator", ";", "Separator"
		string variable_name = getName(node->nodes[0]);
		shared_ptr<PascalVariable> value = getExpressionValue(node->nodes[5], grammar);
		variables.at(variable_name) = cast(value, variables.at(variable_name)->getType());
		break;
	} case 2: { //{"Instruction", {"Writeln", "Separator", "(", "Separator", "Expression", "Separator", ")", ";", "Separator"}}
		shared_ptr<PascalVariable> variable_value = getExpressionValue(node->nodes[4], grammar);
		variable_value->print();
		cout << endl;
		break;
	} case 3: { // "If", "Separator", "Expression", "Separator", "Then",
		         // "Separator", "Instruction", "Separator", "ElsePart"
		shared_ptr<PascalVariable> expression_value = getExpressionValue(node->nodes[2], grammar);
		if (isTrue(expression_value)) {
			interpret(node->nodes[6], grammar);
		} else {
			interpret(node->nodes[8], grammar);
		}
		break;
	} case 4: { // "While", "Separator", "Expression", "Separator", "Do", "Separator", "Instruction"
		while(isTrue(getExpressionValue(node->nodes[2], grammar))) {
			interpret(node->nodes[6], grammar);
		}
		break;
	} case 5: { //"For", "Separator", "Variable", "Separator", ":", "=", "Separator", "Expression", "Separator",
		         // "To", "Separator", "Expression", "Separator", "Do", "Separator", "Instruction"
		string variable_name = getName(node->nodes[2]);
		shared_ptr<PascalVariable> from = getExpressionValue(node->nodes[7], grammar);
		shared_ptr<PascalVariable> to = getExpressionValue(node->nodes[11], grammar);
		if (variables.at(variable_name)->getType() != Integer || from->getType() != Integer ||
				to->getType() != Integer) {
			throw runtime_error("syntax error in 'for' cycle");
		}
		int from_int = *reinterpret_cast<int*>(from->data);
		int to_int = *reinterpret_cast<int*>(to->data);
		for (int i = from_int;; i += ((to_int > from_int) ? 1 : -1)) {
			variables.at(variable_name) = make_shared<PascalInteger>(i);
			interpret(node->nodes[15], grammar);
			if (i == to_int) {
				break;
			}
		}
		break;
	} case 6: { // "Variable", "Separator", "[", "Separator", "Expression", "Separator", "]",
        	     // "Separator", ":", "=", "Separator", "Expression", "Separator", ";", "Separator"
		shared_ptr<PascalVariable> variable = variables.at(getName(node->nodes[0]));
		if (variable->getType() != String) {
			throw runtime_error("invalid [] operand 1");
		}
		shared_ptr<PascalVariable> index = getExpressionValue(node->nodes[4], grammar);
		if (index->getType() != Integer) {
			throw runtime_error("invalid [] operand 2");
		}
		int index_value = *reinterpret_cast<int*>(index->data);
		shared_ptr<PascalVariable> right_value = getExpressionValue(node->nodes[11], grammar);
		right_value = cast(right_value, Char);
		(*reinterpret_cast<string*>(variable->data))[index_value] = *(reinterpret_cast<char*>(right_value->data));
		break;
	} case 7: { // "Else", "Separator", "Instruction", "Separator"
		interpret(node->nodes[2], grammar);
		break;
	} case 8: { // "Write", "Separator", "(", "Separator", "Expression", "Separator", ")", ";", "Separator"
		shared_ptr<PascalVariable> variable_value = getExpressionValue(node->nodes[4], grammar);
			variable_value->print();
			cout << " ";
			break;
	} default: {
		throw runtime_error("unexpected error while interpreting");
		break;
	}
	}
}
