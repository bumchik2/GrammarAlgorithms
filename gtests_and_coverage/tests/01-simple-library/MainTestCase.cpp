#include "MainTestCase.h"
#include "chomsky_to_greybuh.h"
#include "earley.h"
#include "grammar.h"

#include <stdexcept>
#include <vector>
#include <stack>
#include <iostream>

using std::vector;
using std::stack;
using std::stringstream;
using std::ostringstream;

TEST_F(MainTestCase, testIsAlphabetSymbol) {
	EXPECT_TRUE(isAlphabetSymbol("(")); // ( is alphabet symbol
	EXPECT_TRUE(isAlphabetSymbol("a")); // a is alphabet symbol
	EXPECT_FALSE(isAlphabetSymbol("S")); // S is usually a starting symbol, not alphabet
	EXPECT_FALSE(isAlphabetSymbol("epsilon")); // epsilon is not alphabet
}

TEST_F (MainTestCase, testRemoveEpsilon) {
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
	EXPECT_EQ(removeEpsilon(grammar), expected_grammar);
}

TEST_F (MainTestCase, testClassifyRuleChomskyToGreybuh) {
	// S ---> epsilon => 0
	EXPECT_EQ(classifyRuleChomskyToGreybuh(Rule{"S", {"epsilon"}}, "S"), 0);
	// A ---> a => 1
	EXPECT_EQ(classifyRuleChomskyToGreybuh(Rule{"A", {"a"}}, "S"), 1);
	// A ---> B C => 2
	EXPECT_EQ(classifyRuleChomskyToGreybuh(Rule{"A", {"B", "C"}}, "S"), 2);
}

TEST_F (MainTestCase, testChomskyToGreybuh) {
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
	EXPECT_EQ(chomskyToGreybuh(grammar), expected_grammar);
}

TEST_F (MainTestCase, testSituationsOperatorEqual) {
	Rule rule = {"A", {"B", "a"}};
	Situation situation({rule, 0, 1});
	EXPECT_TRUE(situation == situation); // reflexivity test failed for situations ==
	EXPECT_FALSE((situation == Situation{rule, 0, 2}));
			// different positions - different situations
}

TEST_F (MainTestCase, testPrintingSituations) {
	ostringstream os;
	Rule rule = {"A", {"B", "a"}};
	Situation situation({rule, 0, 1});
	os << situation;
	EXPECT_EQ(os.str(), "A--->B a 0 1");
}

TEST_F (MainTestCase, testSituationHash) {
	Rule rule = {"S", {"(", ")"}};
	Situation situation({rule, 0, 1});
	size_t hash0 = SituationHash()(situation);
	EXPECT_EQ(hash0, SituationHash()(situation)); 
	// hash should be the same
	situation.deduced_prefix_length = 1;
	EXPECT_TRUE(hash0 != SituationHash()(situation));
	// situation change should change after changing it's fields
}

TEST_F (MainTestCase, testPredict) {
	Rule rule = {"A", {"B", "a"}};
	int d_number = 0;
	Situation expected_situation(rule, d_number, 0);
	EXPECT_EQ(EarleyAlgorithm().predict_(rule, d_number), expected_situation);
}

TEST_F (MainTestCase, testComplete) {
	Rule rule = {"A", {"B", "a"}};
	Situation situation_k(rule, 0, 0);
	EXPECT_EQ(EarleyAlgorithm().complete_(situation_k), Situation(rule, 0, 1));
}

TEST_F (MainTestCase, testScan) {
	Rule rule = {"A", {"B", "a"}};
	Situation situation({rule, 0, 1});
	Situation expected_situation({rule, 0, 2});
	EXPECT_EQ(EarleyAlgorithm().scan_(situation), expected_situation);
}

TEST_F (MainTestCase, testSituationsUpdating) {
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
	EXPECT_EQ(static_cast<int>(earley_algorithm.D_situations_[0].size()), 1); 
	// we inserted basic situation

	earley_algorithm.predict_(0, grammar, correct_brackets_sequence);
	EXPECT_EQ(static_cast<int>(earley_algorithm.D_situations_[0].size()), 3);
	// (S'-->.S,0), (S-->.,0), (S-->.(S)S,0)

	earley_algorithm.complete_(0, correct_brackets_sequence);
	EXPECT_EQ(static_cast<int>(earley_algorithm.D_situations_[0].size()), 4);
	// (S'-->.S,0), (S-->.,0), (S-->.(S)S,0), (S'-->S,0)

	earley_algorithm.scan_(0, correct_brackets_sequence);
	EXPECT_EQ(static_cast<int>(earley_algorithm.D_situations_[1].size()), 1);

	earley_algorithm.finalize_();
	EXPECT_EQ(static_cast<int>(earley_algorithm.D_situations_.size()), 0); 
	// we inserted basic situation
}

TEST_F (MainTestCase, testIsRecognized) {
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
	EXPECT_TRUE(EarleyAlgorithm().isRecognized(grammar, correct_bracket_sequence));
	// correct bracket sequence test failed
	string incorrect_bracket_sequence = "((()())((())())";
	EXPECT_FALSE(EarleyAlgorithm().isRecognized(grammar, incorrect_bracket_sequence));
	// incorrect bracket sequence test failed
}
