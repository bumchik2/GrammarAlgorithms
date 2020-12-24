#pragma once

#include "grammar.h"

#include <vector>
#include <unordered_set>

using std::vector;
using std::unordered_set;

class Situation {
public:
	Situation(const Rule& rule, int deduced_prefix_length, int position_in_rule):
			rule(rule), deduced_prefix_length(deduced_prefix_length),
			position_in_rule(position_in_rule) {}
	Rule rule;
	int deduced_prefix_length = -1; // standart notation
	int position_in_rule = 0;
};

ostream& operator << (ostream& os, const Situation& s);
bool operator == (const Situation& s1, const Situation& s2);

struct SituationHash {
	size_t operator () (const Situation& s) const;
};

class EarleyAlgorithm {
private:
	vector<unordered_set<Situation, SituationHash>> D_situations_;
	void initialize_(const Grammar& grammar, const string& s);
	void finalize_();

	bool predict_(int d_number, const Grammar& grammar, const string& s);
	Situation predict_(const Rule& rule, int d_number);

	bool complete_(int d_number, const string& s);
	Situation complete_(const Situation& situation_k);

	void scan_(int d_number, const string& s);
	Situation scan_(Situation situation);
public:
	bool isRecognized(const Grammar& grammar, const string& s);
	void print(int d_number);

	friend void testPredict();
	friend void testComplete();
	friend void testScan();
	friend void testSituationsUpdating();
};
