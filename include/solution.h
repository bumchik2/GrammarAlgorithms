#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>

#include "solution.h"

using std::string;
using std::vector;
using std::istream;
using std::ostream;
using std::runtime_error;

struct Rule {
    string from;
    vector<string> to; // symbols are separated with spaces
    // alphabet symbols - lower case English symbols
};

bool isAlphabetSymbol(const string& symbol);
bool operator == (const Rule& rule1, const Rule& rule2);

istream& operator >> (istream& is, Rule& rule);
template<typename T>
ostream& operator << (ostream& os, const vector<T>& v);
ostream& operator << (ostream& os, const Rule& rule);

class Grammar {
public:
    void setStartingSymbol(const string& starting_symbol);
    void addSymbol(const string& symbol);
    void addRule(const Rule& rule);
    void removeRule(const Rule& rule);
    bool containsRule(const Rule& rule) const;

    string starting_symbol;
    vector<string> symbols;
    vector<Rule> rules;
};

bool operator == (const Grammar&, const Grammar&);

istream& operator >> (istream& is, Grammar& grammar);
ostream& operator << (ostream& os, const Grammar& grammar);

Grammar removeEpsilon(const Grammar& grammar);
// this function can only remove epsilon rules after the main
// part of chomsky to greybuh algorithm

int classifyRuleChomskyToGreybuh(const Rule& rule, const string& starting_symbol);
void processRule(Grammar& grammar, const Rule& rule);
Grammar chomskyToGreybuh(const Grammar& grammar);
