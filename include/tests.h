#pragma once
#include "solution.h"
#include "test_runner.h"

void testIsAlphabetSymbol() {
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

void runTests() {
	TestRunner test_runner;
	test_runner.RunTest(testIsAlphabetSymbol, "test determining alphabet symbols");
	test_runner.RunTest(testRemoveEpsilon, "test remove epsilon");
	test_runner.RunTest(testClassifyRuleChomskyToGreybuh, "test rule classifying");
	test_runner.RunTest(testChomskyToGreybuh, "test Chomsky to Greybuh");
}
