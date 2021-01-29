#pragma once

#include "grammar.h"
#include "chomsky_to_greybuh.h"
#include "test_runner.h"
#include "earley.h"
#include "pascal_variable.h"
#include "interpreter.h"
#include "lr_algorithm.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <memory>

using std::to_string;
using std::make_shared;
using std::ifstream;
using std::ostringstream;
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
		{"S", {"a", "B", "B"}},
		{"B", {"a"}},
		{"B", {"epsilon"}},
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
		{"S", {"a", "B"}},
		{"S", {"a", "B", "B"}},
		{"B", {"a"}},
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
	AssertEqual(classifyRuleChomskyToGreybuh(Rule{"D", {"d"}}, "S"), 1);
	// A ---> B C => 2
	AssertEqual(classifyRuleChomskyToGreybuh(Rule{"D", {"B", "C"}}, "S"), 2);
}

void testChomskyToGreybuh() {
	Grammar grammar;
	grammar.setStartingSymbol("S");
	vector<Rule> rules = {
		{"S", {"epsilon"}},
		{"S", {"D", "B"}},
		{"D", {"a"}},
		{"B", {"b"}},
	};
	for (unsigned i = 0; i < rules.size(); ++i) {
		grammar.addRule(rules[i]);
	}

	Grammar expected_grammar;
	expected_grammar.setStartingSymbol("S");
	vector<Rule> expected_rules = {
		{"S", {"epsilon"}},
		{"S", {"a", "D\\S"}},
		{"S", {"b", "B\\S"}},
		{"D\\D", {"a", "D\\B", "S\\D"}},
		{"D\\B", {"a", "D\\B", "S\\B"}},
		{"D\\S", {"a", "D\\B", "S\\S"}},
		{"D\\D", {"b", "B\\B", "S\\D"}},
		{"D\\B", {"b", "B\\B", "S\\B"}},
		{"D\\S", {"b", "B\\B", "S\\S"}},
		{"D\\S", {"a", "D\\B"}},
		{"D\\D", {"b", "S\\D"}},
		{"D\\B", {"b", "S\\B"}},
		{"D\\S", {"b"}},
		{"D\\S", {"b", "B\\B"}},
		{"D\\S", {"b", "S\\S"}},
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
			"Earley correct bracket sequence test failed");
	Assert(LRAlgorithm(grammar).isRecognized(correct_bracket_sequence),
			"LR correct bracket sequence test failed");
	string incorrect_bracket_sequence = "((()())((())())";
	Assert(!EarleyAlgorithm().isRecognized(grammar, incorrect_bracket_sequence),
			"Earley incorrect bracket sequence test failed");
	Assert(!LRAlgorithm(grammar).isRecognized(incorrect_bracket_sequence),
			"LR incorrect bracket sequence test failed");
	Grammar grammar0;
	grammar0.setStartingSymbol("S'");
	vector<Rule> rules0 = {
		{"S'", {"S"}},
		{"S", {"S", "+", "U"}},
		{"S", {"U"}},
		{"U", {"n"}},
		{"U", {"(", "S", ")"}}
	};
	for (unsigned i = 0; i < rules0.size(); ++i) {
		grammar0.addRule(rules0[i]);
	}
	string correct_expression = "n+((n+(n+n+n)+n)+n)+n";
	Assert(EarleyAlgorithm().isRecognized(grammar0, correct_expression),
			"Earley correct expression test failed");
	Assert(LRAlgorithm(grammar0).isRecognized(correct_expression),
			"LR correct expression test failed");
	string incorrect_expression = "n+((n+(n+n+n)+n)";
	Assert(!EarleyAlgorithm().isRecognized(grammar, incorrect_expression),
			"Earley incorrect expression sequence test failed");
	Assert(!LRAlgorithm(grammar0).isRecognized(incorrect_expression),
			"LR incorrect expression sequence test failed");
}

void testLRIsRecognized() {
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
	Grammar grammar0;
	grammar0.setStartingSymbol("S'");
	vector<Rule> rules0 = {
		{"S'", {"S"}},
		{"S", {"S", "+", "U"}},
		{"S", {"U"}},
		{"U", {"n"}},
		{"U", {"(", "S", ")"}}
	};
	for (unsigned i = 0; i < rules0.size(); ++i) {
		grammar0.addRule(rules0[i]);
	}
	string correct_expression = "n+n";
	Assert(EarleyAlgorithm().isRecognized(grammar0, correct_expression),
			"Earley correct expression test failed");
	Assert(LRAlgorithm(grammar0).isRecognized(correct_expression),
			"LR correct expression test failed");
	string incorrect_expression = "n+((n+(n+n+n)+n)";
	Assert(!LRAlgorithm(grammar0).isRecognized(incorrect_expression),
			"LR incorrect expression sequence test failed");

}

void testInterpreterOutput(const string& program, const string& expected_output, 
		int test_number) {
	ostringstream os;
	Grammar grammar;
	ifstream fin1("../data/grammar.txt");
	fin1 >> grammar;
	if (!isRecognized(grammar, program)) {
		throw runtime_error("program is incorrect, bad test " + to_string(test_number));
	}
	Interpreter().interpret(getSyntaxTree(grammar, program), grammar, os);
	AssertEqual(os.str(), expected_output);
}

void testSyntaxTree() {
	Grammar grammar;
	grammar.setStartingSymbol("S");
	vector<Rule> rules = {
		{"S", {}},
		{"S", {"(", "S", ")", "S"}},
	};
	for (unsigned i = 0; i < rules.size(); ++i) {
		grammar.addRule(rules[i]);
	}
	string word = "(()())";
	Assert(EarleyAlgorithm().isRecognized(grammar, word),
			"Earley: (()()) is a correct bracket sequence");
	SyntaxTree syntax_tree = EarleyAlgorithm().getSyntaxTree(grammar, word);
	AssertEqual(getName(syntax_tree.root), word, "S |-- word");
	AssertEqual(syntax_tree.root->rule_number, 1, "rule 1 should be used first");
	Grammar grammar0;
	grammar0.setStartingSymbol("S'");
	vector<Rule> rules0 = {
		{"S'", {"S"}},
		{"S", {"S", "+", "U"}},
		{"S", {"U"}},
		{"U", {"n"}},
		{"U", {"(", "S", ")"}}
	};
	for (unsigned i = 0; i < rules0.size(); ++i) {
		grammar0.addRule(rules0[i]);
	}
	string expression = "n+((n+(n+n)+n)+n+n)+(n+n)";
	Assert(LRAlgorithm(grammar0).isRecognized(expression) &&
			EarleyAlgorithm().isRecognized(grammar0, expression), 
			"both algorithms should recognize the correct expression");
	SyntaxTree syntax_tree1 = EarleyAlgorithm().getSyntaxTree(grammar0, expression);
	SyntaxTree syntax_tree2 = LRAlgorithm(grammar0).getSyntaxTree(expression);
	AssertEqual(syntax_tree1, syntax_tree2);
}

void testPascalVariables() {
	AssertEqual(Real, leastCommonType(Integer, Real));
	AssertEqual(Boolean, leastCommonType(Boolean, Boolean));
	AssertEqual(String, leastCommonType(String, Char));
	Assert(isTrue(isLess(make_shared<PascalInteger>(1), make_shared<PascalReal>(1.5))), "1 < 1.5");
	Assert(isTrue(isGreater(make_shared<PascalReal>(-1), make_shared<PascalReal>(-1.5))), "-1 > -1.5");
	Assert(isTrue(areEqual(sum(make_shared<PascalInteger>(2), make_shared<PascalInteger>(3)),
			make_shared<PascalReal>(5))), "2 + 3 = 5");
	Assert(isTrue(areEqual(multiply(make_shared<PascalInteger>(2), make_shared<PascalInteger>(3)),
			make_shared<PascalReal>(6))), "2 * 3 = 6");
	Assert(isTrue(areEqual(diff(make_shared<PascalReal>(-4.5), make_shared<PascalInteger>(2)),
			make_shared<PascalReal>(-6.5))), "-4.5 - 2 = -6.5");
	Assert(isTrue(areEqual(sum(make_shared<PascalString>("xyz"), make_shared<PascalChar>('a')),
			make_shared<PascalString>("xyza"))), "'xyz' + 'a' = 'xyza'");
	Assert(isTrue(areEqual(divide(make_shared<PascalReal>(7), make_shared<PascalReal>(20)),
			make_shared<PascalReal>(0.35))), "7 / 20 = 0.35");
	Assert(isTrue(areEqual(negative(make_shared<PascalReal>(7)), make_shared<PascalReal>(-7))),
			"- 7 = -7");
	Assert(isTrue(make_shared<PascalInteger>(1)), "1 should be casted to True");
}

struct interpreterTest {
	string program;
	string expected_output;
};

void testInterpreter() {
	const vector<interpreterTest> tests = {
		{"begin end.", ""},
		{"begin writeln('hello, world!'); end.", "hello, world!\n"},
		{"begin writeln(2 + 2 * 2); end.", "6\n"},
		{"begin writeln(True); end.", "True\n"},
		{"var i: integer; begin for i := 1 to 5 do write(i); end.", 
				"1 2 3 4 5 "},
		{"var i: integer; begin i := 5;"
			"while i > 0 do begin writeln(i * i); i := i - 1; end; end.", 
			"25\n16\n9\n4\n1\n"},
		{"var i: integer; begin i := 5;"
				"if False then writeln(1); else writeln(0); end.", 
				"0\n"},
		{"var begin write(Not (Not True Or Not(False And True And False Or True)));"
				"end.", "True "},
		{"var begin write(1.0 / 10); end.", "0.1 "},
		{"var s:string;begin s := 'abcd'; write(s[0]); s[0] := 'x'; writeln(s[0]);"
				"end.", "a x\n"},
		{"var s: string; begin s := 'ab'; s := s + s; s := s + s; writeln(s);"
				"end.", "abababab\n"},
		{"var sa, sb: string; begin sa := 'ab'; sb := 'cd'; writeln((sa+sb)[3]);"
				"end.", "d\n"}
	};
	for (unsigned i = 0; i < tests.size(); ++i) {
		testInterpreterOutput(tests[i].program, tests[i].expected_output, i);
	}
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
	test_runner.RunTest(testIsRecognized, "test 'is recognized'");
	test_runner.RunTest(testSyntaxTree, "test syntax tree");
	test_runner.RunTest(testPascalVariables, "test pascal variable");
	test_runner.RunTest(testInterpreter, "test pascal interpreter");
}
