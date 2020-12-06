#include "grammar.h"
#include "earley.h"

#include <vector>
#include <algorithm>
#include <unordered_set>
#include <iostream>

using std::cout;
using std::endl;
using std::vector;
using std::find;
using std::unordered_set;

ostream& operator << (ostream& os, const Situation& s) {
	os << s.rule << ' ' << s.deduced_prefix_length << ' ' << s.position_in_rule;
	return os;
}

size_t SituationHash::operator () (const Situation& s) const {
	size_t hash_1 = std::hash<int>()(s.deduced_prefix_length);
	size_t hash_2 = std::hash<int>()(s.position_in_rule);
	return hash_1 ^ (hash_2 << 1);
}

bool operator == (const Situation& s1, const Situation& s2) {
	return s1.rule == s2.rule && s1.deduced_prefix_length == s2.deduced_prefix_length &&
			s1.position_in_rule == s2.position_in_rule;
}

void EarleyAlgorithm::initialize_(const Grammar& grammar, const string& s) {
	D_situations_ = vector<unordered_set<Situation, SituationHash>>(s.size() + 1);
	Rule basic_rule = {"S'", {"S"}}; // (S'->.S, 0) situation
	D_situations_[0].insert({basic_rule, 0, 0});
}

Situation EarleyAlgorithm::predict_(const Rule& rule, int d_number) {
	return Situation(rule, d_number, 0);
}

bool EarleyAlgorithm::predict_(int d_number, const Grammar& grammar, const string& s) {
	// return true if a new situation appeared
	bool new_situation_appeared = false;
	for (const auto& situation : D_situations_[d_number]) {
		if (situation.position_in_rule >= static_cast<int>(situation.rule.to.size())) {
			continue;
		}
		string next_symbol = situation.rule.to[situation.position_in_rule];
		if (!isAlphabetSymbol(next_symbol)) {
			for (unsigned rule_number = 0; rule_number < grammar.rules.size(); ++rule_number) {
				if (grammar.rules[rule_number].from == next_symbol) {
					Situation new_situation = predict_(grammar.rules[rule_number], d_number);
					auto insert_result = D_situations_[d_number].insert(new_situation);
					new_situation_appeared |= insert_result.second;
				}
			}
		}
	}
	return new_situation_appeared;
}

Situation EarleyAlgorithm::complete_(const Situation& situation_k) {
	return Situation(situation_k.rule, situation_k.deduced_prefix_length,
			situation_k.position_in_rule + 1);
}

bool EarleyAlgorithm::complete_(int d_number, const string& s) {
	// return true if new situation appeared
	bool new_situation_appeared = false;
	for (const auto& situation_j : D_situations_[d_number]) {
		if (situation_j.position_in_rule != static_cast<int>(situation_j.rule.to.size())) {
			continue;
		}
		for (const auto& situation_k : D_situations_[situation_j.deduced_prefix_length]) {
			if (situation_k.position_in_rule == static_cast<int>(situation_k.rule.to.size())) {
				continue;
			}
			if (situation_k.rule.to[situation_k.position_in_rule] != situation_j.rule.from) {
				continue;
			}
			Situation new_situation = complete_(situation_k);
			auto insert_result = D_situations_[d_number].insert(new_situation);
			new_situation_appeared |= insert_result.second;
		}
	}
	return new_situation_appeared;
}

Situation EarleyAlgorithm::scan_(Situation situation) {
	++situation.position_in_rule;
	return situation;
}

void EarleyAlgorithm::scan_(int d_number, const string& s) {
	for (const auto& situation : D_situations_[d_number]) {
		if (situation.position_in_rule < static_cast<int>(situation.rule.to.size()) &&
				isAlphabetSymbol(situation.rule.to[situation.position_in_rule])) {
			string current_character;
			current_character += s[d_number];
			if (current_character != situation.rule.to[situation.position_in_rule]) {
				continue;
			}
			Situation new_situation = scan_(situation);
			D_situations_[d_number + 1].insert(new_situation);
		}
	}
}

void EarleyAlgorithm::finalize_() {
	D_situations_.clear();
}

bool EarleyAlgorithm::isRecognized(const Grammar& grammar, const string& s) {
	// we expect grammar to have a S' starting symbol and S'->S basic rule
	initialize_(grammar, s);

	bool something_changed = true;
	while(something_changed) {
		something_changed = false;
		something_changed |= predict_(0, grammar, s);
		something_changed |= complete_(0, s);
	}

	for (unsigned i = 1; i <= s.size(); ++i) {
		scan_(i - 1, s);
		bool something_changed = true;
		while(something_changed) {
			something_changed = false;
			something_changed |= predict_(i, grammar, s);
			something_changed |= complete_(i, s);
		}
	}

	Rule desired_rule = {"S'", {"S"}};
	Situation desired_situation(desired_rule, 0, 1); // (S'->S., 0) situation
	bool answer = std::find(D_situations_[s.size()].begin(), D_situations_[s.size()].end(),
			desired_situation) != D_situations_[s.size()].end();
	finalize_();
	return answer;
}
