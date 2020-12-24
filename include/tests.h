#pragma once

#include "grammar.h"
#include "chomsky_to_greybuh.h"
#include "test_runner.h"
#include "earley.h"

#include <iostream>

using std::cout;
using std::endl;

void testIsAlphabetSymbol() {
	Assert(isAlphabetSymbol("("), "( is alphabet symbol");
	Assert(isAlphabetSymbol("a"), "a is alphabet symbol");
	Assert(!isAlphabetSymbol("S"), "S is usually a starting symbol, not alphabet");
	Assert(!isAlphabetSymbol("epsilon"), "epsilon is not alphabet");
}

void testRemoveEpsilon() {
	Grammar grammar;
	grammar.setStartingSymbol("S'");
	vector<Rule> rules = {
		{"S'", {"epsilon"}},
		{"S'", {"b", "S"}},
		{"S", {"a", "A", "A"}},
		{"A", {"a"}},
		{"A", {"epsilon"}},
	};
	for (unsigned i = 0; i < rules.size(); ++i) {
		grammar.addRule(rules[i]);
	}

	Grammar expected_grammar;
	expected_grammar.setStartingSymbol("S'");
	vector<Rule> expected_rules = {
		{"S'", {"epsilon"}},
		{"S'", {"b", "S"}},
		{"S", {"a"}},
		{"S", {"a", "A"}},
		{"S", {"a", "A", "A"}},
		{"A", {"a"}},
	};
	for (unsigned i = 0; i < expected_rules.size(); ++i) {
		expected_grammar.addRule(expected_rules[i]);
	}
	AssertEqual(removeEpsilon(grammar), expected_grammar);
}

void testClassifyRuleChomskyToGreybuh() {
	// S ---> epsilon => 0
	AssertEqual(classifyRuleChomskyToGreybuh(Rule{"S", {"epsilon"}}, "S"), 0);
	// A ---> a => 1
	AssertEqual(classifyRuleChomskyToGreybuh(Rule{"A", {"a"}}, "S"), 1);
	// A ---> B C => 2
	AssertEqual(classifyRuleChomskyToGreybuh(Rule{"A", {"B", "C"}}, "S"), 2);
}

void testChomskyToGreybuh() {
	Grammar grammar;
	grammar.setStartingSymbol("S");
	vector<Rule> rules = {
		{"S", {"epsilon"}},
		{"S", {"A", "B"}},
		{"A", {"a"}},
		{"B", {"b"}},
	};
	for (unsigned i = 0; i < rules.size(); ++i) {
		grammar.addRule(rules[i]);
	}

	Grammar expected_grammar;
	expected_grammar.setStartingSymbol("S");
	vector<Rule> expected_rules = {
		{"S", {"epsilon"}},
		{"S", {"a", "A\\S"}},
		{"S", {"b", "B\\S"}},
		{"A\\A", {"a", "A\\B", "S\\A"}},
		{"A\\B", {"a", "A\\B", "S\\B"}},
		{"A\\S", {"a", "A\\B", "S\\S"}},
		{"A\\A", {"b", "B\\B", "S\\A"}},
		{"A\\B", {"b", "B\\B", "S\\B"}},
		{"A\\S", {"b", "B\\B", "S\\S"}},
		{"A\\S", {"a", "A\\B"}},
		{"A\\A", {"b", "S\\A"}},
		{"A\\B", {"b", "S\\B"}},
		{"A\\S", {"b"}},
		{"A\\S", {"b", "B\\B"}},
		{"A\\S", {"b", "S\\S"}},
	};
	for (unsigned i = 0; i < expected_rules.size(); ++i) {
		expected_grammar.addRule(expected_rules[i]);
	}
	AssertEqual(chomskyToGreybuh(grammar), expected_grammar);
}

void testSituationsOperatorEqual() {
	Rule rule = {"A", {"B", "a"}};
	Situation situation({rule, 0, 1});
	Assert(situation == situation, "reflexivity test failed for situations ==");
	Assert(!(situation == Situation{rule, 0, 2}),
			"different positions - different situations");
}

void testPrintingSituations() {
	ostringstream os;
	Rule rule = {"A", {"B", "a"}};
	Situation situation({rule, 0, 1});
	os << situation;
	AssertEqual(os.str(), "A--->B a 0 1");
}

void testSituationHash() {
	Rule rule = {"S", {"(", ")"}};
	Situation situation({rule, 0, 1});
	size_t hash0 = SituationHash()(situation);
	AssertEqual(hash0, SituationHash()(situation)); // hash should be the same
	situation.deduced_prefix_length = 1;
	Assert(hash0 != SituationHash()(situation),
			"situation change should change after changing it's fields");
}

void testPredict() {
	Rule rule = {"A", {"B", "a"}};
	int d_number = 0;
	Situation expected_situation(rule, d_number, 0);
	AssertEqual(EarleyAlgorithm().predict_(rule, d_number), expected_situation);
}

void testComplete() {
	Rule rule = {"A", {"B", "a"}};
	Situation situation_k(rule, 0, 0);
	AssertEqual(EarleyAlgorithm().complete_(situation_k), Situation(rule, 0, 1));
}

void testScan() {
	Rule rule = {"A", {"B", "a"}};
	Situation situation({rule, 0, 1});
	Situation expected_situation({rule, 0, 2});
	AssertEqual(EarleyAlgorithm().scan_(situation), expected_situation);
}

void testSituationsUpdating() {
	Grammar grammar;
	grammar.setStartingSymbol("S'");
	vector<Rule> rules = {
		{"S'", {"S"}},
		{"S", {}},
		{"S", {"(", "S", ")", "S"}}
	};
	for (unsigned i = 0; i < rules.size(); ++i) {
		grammar.addRule(rules[i]);
	}
	string correct_brackets_sequence = "(())()";

	EarleyAlgorithm earley_algorithm;
	earley_algorithm.initialize_(grammar, correct_brackets_sequence);
	AssertEqual(static_cast<int>(earley_algorithm.D_situations_[0].size()), 1); // we inserted basic situation

	earley_algorithm.predict_(0, grammar, correct_brackets_sequence);
	AssertEqual(static_cast<int>(earley_algorithm.D_situations_[0].size()), 3);
	// (S'-->.S,0), (S-->.,0), (S-->.(S)S,0)

	earley_algorithm.complete_(0, correct_brackets_sequence);
	AssertEqual(static_cast<int>(earley_algorithm.D_situations_[0].size()), 4);
	// (S'-->.S,0), (S-->.,0), (S-->.(S)S,0), (S'-->S,0)

	earley_algorithm.scan_(0, correct_brackets_sequence);
	AssertEqual(static_cast<int>(earley_algorithm.D_situations_[1].size()), 1);

	earley_algorithm.finalize_();
	AssertEqual(static_cast<int>(earley_algorithm.D_situations_.size()), 0); // we inserted basic situation
}

void testIsRecognized() {
	Grammar grammar;
	grammar.setStartingSymbol("S'");
	vector<Rule> rules = {
		{"S'", {"S"}},
		{"S", {}},
		{"S", {"(", "S", ")", "S"}}
	};
	for (unsigned i = 0; i < rules.size(); ++i) {
		grammar.addRule(rules[i]);
	}
	string correct_bracket_sequence = "((()())(()())())";
	Assert(EarleyAlgorithm().isRecognized(grammar, correct_bracket_sequence),
			"correct bracket sequence test failed");
	string incorrect_bracket_sequence = "((()())((())())";
	Assert(!EarleyAlgorithm().isRecognized(grammar, incorrect_bracket_sequence),
				"incorrect bracket sequence test failed");
}

void runTests() {
	TestRunner test_runner;
	test_runner.RunTest(testIsAlphabetSymbol, "test determining alphabet symbols");
	test_runner.RunTest(testRemoveEpsilon, "test remove epsilon");
	test_runner.RunTest(testClassifyRuleChomskyToGreybuh, "test rule classifying");
	test_runner.RunTest(testChomskyToGreybuh, "test Chomsky to Greybuh");
	test_runner.RunTest(testSituationsOperatorEqual, "test operator == for situations");
	test_runner.RunTest(testPrintingSituations, "test printing situations");
	test_runner.RunTest(testSituationHash, "test situations hash");
	test_runner.RunTest(testPredict, "test predict in earley algorithm");
	test_runner.RunTest(testComplete, "test complete in earley algorithm");
	test_runner.RunTest(testScan, "test scan in earley algorithm");
	test_runner.RunTest(testSituationsUpdating, "test situations updating");
	test_runner.RunTest(testIsRecognized, "test earley algorithm 'is recognized' function");
}
