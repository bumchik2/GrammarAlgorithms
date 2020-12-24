#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <algorithm>

#include "grammar.h"
#include "chomsky_to_greybuh.h"

using std::string;
using std::vector;
using std::istream;
using std::ostream;
using std::runtime_error;
using std::find;
using std::endl;
using std::cout;

void processRule(Grammar& grammar, const Rule& rule) {
	vector<bool> is_epsilon_generating(rule.to.size(), false);
	vector<int> epsilon_generating;
	for (unsigned i = 0; i < rule.to.size(); ++i) {
		if (rule.to[i] == grammar.starting_symbol) {
			continue;
		}
		Rule epsilon_rule {rule.to[i], {"epsilon"}};
		if (grammar.containsRule(epsilon_rule)) {
			is_epsilon_generating[i] = true;
			epsilon_generating.push_back(i);
		}
	}

	if (epsilon_generating.size() == rule.to.size()) {
		throw runtime_error("we don't want to process such rules");
	}

	vector<bool> symbol_is_left (rule.to.size(), false);
	for (int mask = 0; mask < (1 << epsilon_generating.size()); ++mask) {
		int mask_copy = mask;
		for (unsigned int i = 0; i < epsilon_generating.size(); ++i) {
			symbol_is_left[epsilon_generating[i]] = mask_copy % 2;
			mask_copy /= 2;
		}

		vector<string> new_to;
		for (unsigned i = 0; i < rule.to.size(); ++i) {
			if (!is_epsilon_generating[i] || !symbol_is_left[i]) {
				new_to.push_back(rule.to[i]);
			}
		}

		grammar.addRule(Rule{rule.from, new_to});
	}
}

Grammar removeEpsilon(const Grammar& grammar) {
    Grammar result_grammar = grammar;
    for (unsigned rule_number = 0; rule_number < grammar.rules.size(); ++rule_number) {
    	processRule(result_grammar, grammar.rules[rule_number]);
    }

    for (unsigned symbol_number = 0; symbol_number < result_grammar.symbols.size();
    		++symbol_number) {
        string symbol = grammar.symbols[symbol_number];
        if (symbol == grammar.starting_symbol) {
        	continue;
        }
        Rule epsilon_rule {symbol, {"epsilon"}};
        if (result_grammar.containsRule(epsilon_rule)) {
            result_grammar.removeRule(epsilon_rule);
        }
    }
    return result_grammar;
}

int classifyRuleChomskyToGreybuh(const Rule& rule, const string& starting_symbol) {
	if (rule.from == starting_symbol && rule.to.size() == 1 && rule.to[0] == "epsilon") {
		return 0;
	}
    if (rule.to.size() > 2 || rule.to.size() == 0) {
        throw runtime_error("rule in classify function is not in Chomsky form 1");
    }
    if (rule.to.size() == 1 && (!isAlphabetSymbol(rule.to[0]) || (rule.to[0] == "epsilon"))) {
        throw runtime_error("rule in classify function is not in Chomsky form 2");
    }
    if (rule.to.size() == 2 &&
            (isAlphabetSymbol(rule.to[0]) || isAlphabetSymbol(rule.to[1]))) {
        throw runtime_error("rule in classify function is not in Chomsky form 3");
    }
    return rule.to.size();
}

Grammar chomskyToGreybuh(const Grammar& grammar) {
    Grammar result_grammar;
    result_grammar.setStartingSymbol(grammar.starting_symbol);

    // A\A--->epsilon
    for (unsigned int i = 0; i < grammar.symbols.size(); ++i) {
        result_grammar.addRule({
            grammar.symbols[i] + "\\" + grammar.symbols[i],
            {"epsilon"}
        });
    }

    for (unsigned rule_number = 0; rule_number < grammar.rules.size(); ++rule_number) {
        Rule rule = grammar.rules[rule_number];
        if (classifyRuleChomskyToGreybuh(rule, grammar.starting_symbol) == 0) {
        	// starting_symbol ---> epsilon rule
        	result_grammar.addRule(rule);
        }
        if (classifyRuleChomskyToGreybuh(rule, grammar.starting_symbol) == 1) {
            result_grammar.addRule({
                grammar.starting_symbol,
                {rule.to[0], rule.from + "\\" + grammar.starting_symbol}
            });
        }
    }

    for (unsigned rule1_number = 0; rule1_number < grammar.rules.size(); ++rule1_number) {
        Rule rule1 = grammar.rules[rule1_number];
        if (classifyRuleChomskyToGreybuh(rule1, grammar.starting_symbol) != 1) {
            continue;
        }
        for (unsigned rule2_number = 0; rule2_number < grammar.rules.size(); ++rule2_number) {
            Rule rule2 = grammar.rules[rule2_number];
            if (classifyRuleChomskyToGreybuh(rule2, grammar.starting_symbol) != 2) {
                continue;
            }
            for (unsigned B_symbol_number = 0; B_symbol_number < grammar.symbols.size();
                    ++B_symbol_number) {
                string A_symbol = rule2.to[0];
                string B_symbol = grammar.symbols[B_symbol_number];
                string C_symbol = rule2.from;
                string D_symbol = rule2.to[1];
                string E_symbol = rule1.from;
                string e_symbol = rule1.to[0];
                result_grammar.addRule({
                    A_symbol + "\\" + B_symbol,
                    {e_symbol, E_symbol + "\\" + D_symbol, C_symbol + "\\" + B_symbol}
                });
            }
        }
    }
    return removeEpsilon(result_grammar);
}
