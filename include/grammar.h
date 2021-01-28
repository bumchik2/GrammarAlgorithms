#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>

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

class RuleHash {
public:
    size_t operator() (const Rule& rule) const {
        size_t hash1 = std::hash<size_t>()(rule.from.size());
        size_t hash2 = std::hash<size_t>()(rule.to.size());
        return hash1 ^ (hash2 << 1);
    }
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
    void addAlphabetSymbol(const string& alphabet_symbol);
    void addRule(const Rule& rule);
    void removeRule(const Rule& rule);
    bool containsRule(const Rule& rule) const;

    string starting_symbol;
    vector<string> symbols;
    vector<string> alphabet_symbols;
    vector<Rule> rules;
};

bool operator == (const Grammar&, const Grammar&);

istream& operator >> (istream& is, Grammar& grammar);
ostream& operator << (ostream& os, const Grammar& grammar);
