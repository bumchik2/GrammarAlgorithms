#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>

#include "grammar.h"

using std::string;
using std::vector;
using std::istream;
using std::ostream;
using std::runtime_error;
using std::find;
using std::endl;
using std::cout;
using std::unordered_set;
using std::unordered_map;

const unordered_set<string> special_alphabet_symbols = {
    "(", ")", "{", "}", "{", "}", ";", ":", ".", ",", "[", "]", "O", "A",
    " ", "\n", "*", "+", "-", "/", "=", "<", ">", "'", "F", "T", "N", "!"
};

const unordered_map<string, string> special_alias = {
    {"space", " "},
    {"newline", "\n"}
};

bool isAlphabetSymbol(const string& symbol) {
    return (symbol.size() == 1 && symbol[0] >= 'a' && symbol[0] <= 'z') ||
            (symbol.size() == 1 && symbol[0] >= '0' && symbol[0] <= '9') ||
            special_alphabet_symbols.find(symbol) != special_alphabet_symbols.end();
}

bool operator == (const Rule& rule1, const Rule& rule2) {
    return rule1.from == rule2.from && rule1.to == rule2.to;
}

istream& operator >> (istream& is, Rule& rule) {
    is >> rule.from;
    int symbols_number;
    is >> symbols_number;
    rule.to = vector<string>(symbols_number);
    for (int i = 0; i < symbols_number; ++i) {
        is >> rule.to[i];
        if (special_alias.find(rule.to[i]) != special_alias.end()) {
            rule.to[i] = special_alias.at(rule.to[i]);
        }
    }
    return is;
}

template<typename T>
ostream& operator << (ostream& os, const vector<T>& v) {
    for (unsigned i = 0; i < v.size(); ++i) {
        if (i != 0) {
            os << ' ';
        }
        os << v[i];
    }
    return os;
}

ostream& operator << (ostream& os, const Rule& rule) {
    os << rule.from << "--->" << rule.to;
    return os;
}

void Grammar::setStartingSymbol(const string& new_starting_symbol) {
    if (isAlphabetSymbol(starting_symbol)) {
        throw runtime_error("alphabet symbol can't be starting");
    }
    addSymbol(new_starting_symbol);
    starting_symbol = new_starting_symbol;
}

void Grammar::addSymbol(const string& symbol) {
    if (symbol == "epsilon") {
        return;
    }
    if (isAlphabetSymbol(symbol)) {
        return;
    }
    if (find(symbols.begin(), symbols.end(), symbol) == symbols.end()) {
        symbols.push_back(symbol);
    }
}

void Grammar::addRule(const Rule& rule) {
    if (find(rules.begin(), rules.end(), rule) == rules.end()) {
        rules.push_back(rule);
        addSymbol(rule.from);
        for (unsigned i = 0; i < rule.to.size(); ++i) {
            addSymbol(rule.to[i]);
        }
    }
}

void Grammar::removeRule(const Rule& rule) {
    vector<Rule>::iterator rule_iterator = find(
            rules.begin(), rules.end(), rule);
    if (rule_iterator == rules.end()) {
        throw runtime_error("trying to remove non-existing rule");
    }
    rules.erase(rule_iterator);
}

bool Grammar::containsRule(const Rule& rule) const {
    return find(rules.begin(), rules.end(), rule) != rules.end();
}

bool operator == (const Grammar& grammar1, const Grammar& grammar2) {
    if (grammar1.starting_symbol != grammar2.starting_symbol) {
        return false;
    }
    if (grammar1.rules.size() != grammar2.rules.size()) {
        return false;
    }
    for (unsigned rule_number = 0; rule_number < grammar1.rules.size(); ++rule_number) {
        Rule rule = grammar1.rules[rule_number];
        if (find(grammar2.rules.begin(), grammar2.rules.end(), rule) ==
                grammar2.rules.end()) {
            return false;
        }
    }
    return true;
}

istream& operator >> (istream& is, Grammar& grammar) {
    string starting_symbol;
    is >> starting_symbol;
    grammar.setStartingSymbol(starting_symbol);
    int rules_number;
    is >> rules_number;
    for (int i = 0; i < rules_number; ++i) {
        Rule new_rule;
        is >> new_rule;
        grammar.addRule(new_rule);
    }
    return is;
}

ostream& operator << (ostream& os, const Grammar& grammar) {
    os << "Starting symbol: " << grammar.starting_symbol << endl;
    os << grammar.rules.size() << " rules:" << endl;
    for (unsigned i = 0; i < grammar.rules.size(); ++i) {
        if (i != 0) {
            os << endl;
        }
        os << grammar.rules[i];
    }
    return os;
}
