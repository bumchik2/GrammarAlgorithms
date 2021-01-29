#include "grammar.h"
#include "lr_algorithm.h"

#include <iostream>
#include <vector>

using std::vector;
using std::cout;
using std::cin;
using std::endl;

int main() {
	Grammar grammar;
	grammar.setStartingSymbol("S'");
	/*vector<Rule> rules = {
		{"S'", {"S"}},
		{"S", {"S", "+", "U"}},
		{"S", {"U"}},
		{"U", {"n"}},
		{"U", {"(", "S", ")"}}
	};*/
	vector<Rule> rules = {
		{"S'", {"S"}},
		{"S", {}},
		{"S", {"(", "S", ")", "S"}}
	};
	for (unsigned i = 0; i < rules.size(); ++i) {
		grammar.addRule(rules[i]);
	}

	LRAlgorithm lr_algorithm(grammar);

	while(cin) {
		string expression;
		cin >> expression;
		bool is_recognized = lr_algorithm.isRecognized(expression);
		cout << is_recognized << endl;
	}
}