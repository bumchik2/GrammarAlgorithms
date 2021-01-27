#pragma once

#include "grammar.h"
#include "syntax_tree.h"

#include <vector>
#include <unordered_set>
#include <unordered_map>

using std::vector;
using std::unordered_set;
using std::unordered_map;

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

struct SituationAndNumber {
	SituationAndNumber() = delete;
	Situation situation;
	int number;
};

bool operator == (const SituationAndNumber& , const SituationAndNumber&);

struct SituationAndNumberHash {
	size_t operator() (const SituationAndNumber& situation_and_number) const {
		return SituationHash()(situation_and_number.situation) +
				static_cast<size_t>(situation_and_number.number);
	}
};

struct ParentSituations {
	unordered_map<SituationAndNumber, SituationAndNumber, SituationAndNumberHash>
		scan_parent, complete_parent1, complete_parent2;
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
	ParentSituations parent_situations; // we keep it to be able to build syntax tree

	SyntaxTree::Node* getSyntaxTreeVertex(SituationAndNumber, const Grammar& grammar);
	SyntaxTree getSyntaxTree(const Grammar& grammar, const string& s);
	bool isRecognized(const Grammar& grammar, const string& s);
	void print(int d_number);

	friend void testPredict();
	friend void testComplete();
	friend void testScan();
	friend void testSituationsUpdating();
};
