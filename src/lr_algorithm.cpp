#include "grammar.h"
#include "syntax_tree.h"
#include "lr_algorithm.h"

#include <algorithm>
#include <vector>
#include <iostream>
#include <stack>

using std::reverse;
using std::cout;
using std::endl;
using std::find;
using std::vector;
using std::stack;

bool operator == (const LRSituation& lr_situation1, const LRSituation& lr_situation2) {
	return (lr_situation1.rule == lr_situation2.rule &&
			lr_situation1.position_in_rule == lr_situation2.position_in_rule);
}

bool operator == (const LRState& state1, const LRState& state2) {
	return state1.situations == state2.situations;
}

ostream& operator << (ostream& os, const LRSituation& lr_situation) {
	os << "(" << lr_situation.rule << ", " << lr_situation.position_in_rule << ")";
	return os;
}

ostream& operator << (ostream& os, const LRState& lr_state) {
	for (const LRSituation& lr_situation: lr_state.situations) {
		os << lr_situation << ' ';
	}
	return os;
}

void LRAlgorithm::printTable() const {
	for (unsigned i = 0; i < table.size(); ++i) {
		for (const string& symbol: grammar.symbols) {
			cout << "(" << LRCellTypeToString.at(table[i].at(symbol).cell_type) <<
					", " << table[i].at(symbol).number << ") ";
		}
		for (const string& symbol: grammar.alphabet_symbols) {
			cout << "(" << LRCellTypeToString.at(table[i].at(symbol).cell_type) <<
					", " << table[i].at(symbol).number << ") ";
		}
		cout << endl;
	}
}

vector<LRSituation> LRPredict(const LRSituation& lr_situation, const Grammar& grammar) {
	string symbol = lr_situation.rule.to[lr_situation.position_in_rule];
	vector<LRSituation> answer;
	for (unsigned i = 0; i < grammar.rules.size(); ++i) {
		if (grammar.rules[i].from == symbol) {
			answer.push_back(LRSituation(grammar.rules[i], 0));
		}
	}
	return answer;
}

LRState Closure(LRState lr_state, const Grammar& grammar) {
	bool something_changed = true;
	while(something_changed) {
		unsigned old_size = lr_state.situations.size();
		vector<LRSituation> situations_to_add;
		for (const LRSituation& lr_situation: lr_state.situations) {
			if (static_cast<int>(lr_situation.rule.to.size()) == lr_situation.position_in_rule) {
				continue;
			}
			if (!isAlphabetSymbol(lr_situation.rule.to[lr_situation.position_in_rule])) {
				vector<LRSituation> new_situations = LRPredict(lr_situation, grammar);
				for (unsigned i = 0; i < new_situations.size(); ++i) {
					situations_to_add.push_back(new_situations[i]);
				}
			}
		}
		for (unsigned i = 0; i < situations_to_add.size(); ++i) {
			lr_state.situations.insert(situations_to_add[i]);
		}
		something_changed = (lr_state.situations.size() > old_size);
	}
	return lr_state;
}

LRState GoTo(const LRState& lr_state, const string& symbol, const Grammar& grammar) {
	LRState new_lr_state;
	for (const LRSituation& lr_situation: lr_state.situations) {
		if (static_cast<int>(lr_situation.rule.to.size()) == lr_situation.position_in_rule) {
			continue;
		}
		string current_symbol = lr_situation.rule.to[lr_situation.position_in_rule];
		if (symbol == current_symbol) {
			new_lr_state.situations.insert(LRSituation(lr_situation.rule, 
					lr_situation.position_in_rule + 1));
		}
	}
	return Closure(new_lr_state, grammar);
}

LRCell LRAlgorithm::getCell(int state_number, const string& symbol) const {
	for (unsigned other_state_number = 0; other_state_number < table.size();
			++other_state_number) {
		if (states[other_state_number] == GoTo(states[state_number], symbol, grammar)) {
			return {shift, static_cast<int>(other_state_number)};
		}
	}
	for (unsigned rule_number = 0; rule_number < grammar.rules.size(); ++rule_number) {
		if (find(states[state_number].situations.begin(), states[state_number].situations.end(),
				LRSituation(grammar.rules[rule_number], grammar.rules[rule_number].to.size())) !=
				states[state_number].situations.end()) {
			return {reduce, static_cast<int>(rule_number)};
		}
	}
	return {error, -1};
}

void LRAlgorithm::createTable() {
	LRSituation start_situation = LRSituation(Rule{"S'", {"S"}}, 0);
	states.push_back(Closure(LRState(unordered_set<LRSituation, LRSituationHash>
			{start_situation}), grammar));
	LRDFS(0);

	table.resize(states.size());

	for (unsigned state_number = 0; state_number < states.size(); ++state_number) {
		for (const string& symbol : grammar.symbols) {
			table[state_number][symbol] = getCell(state_number, symbol);
		}
		for (const string& alphabet_symbol : grammar.alphabet_symbols) {
			table[state_number][alphabet_symbol] = getCell(state_number, alphabet_symbol);
		}
	}
}

void LRAlgorithm::LRDFS(int state_number) {
	for (const string& symbol: grammar.symbols) {
		LRState new_state = GoTo(states[state_number], symbol, grammar);
		if (find(states.begin(), states.end(), new_state) == states.end() &&
					new_state.situations.size() != 0) {
			states.push_back(new_state);
			LRDFS(states.size() - 1);
		}
	}
	for (const string& symbol: grammar.alphabet_symbols) {
		LRState new_state = GoTo(states[state_number], symbol, grammar);
		if (find(states.begin(), states.end(), new_state) == states.end() &&
					new_state.situations.size() != 0) {
			states.push_back(new_state);
			LRDFS(states.size() - 1);
		}
	}
}

struct EdgeVertex {
	string symbol;
	int state_number;
};

ostream& operator << (ostream& os, const EdgeVertex& edge_vertex) {
	os << edge_vertex.symbol << ", " << edge_vertex.state_number;
	return os;
}

template<typename T>
vector<T> stack_to_vector(stack<T> st) {
	vector<T> v;
	while(!st.empty()) {
		v.push_back(st.top());
		st.pop();
	}
	reverse(v.begin(), v.end());
	return v;
}

template<typename T>
ostream& operator << (ostream& os, const vector<T>& v) {
	for (unsigned i = 0; i < v.size(); ++i) {
		if (i != 0) {
			os << ", ";
		}
		os << v[i];
	}
	return os;
}

template<typename T>
ostream& operator << (ostream& os, const stack<T>& st) {
	os << stack_to_vector(st);
	return os;
}

bool LRAlgorithm::isRecognized(string s) {
	s += "$";
	stack<EdgeVertex> path;
	path.push({"", 0});
	int current_position = 0;
	while(true) {
		string current_symbol;
		current_symbol += s[current_position];
		LRCell cell = table[path.top().state_number][current_symbol];
		if (cell.cell_type == error) {
			return false;
		} else if (cell.cell_type == shift) {
			if (current_symbol == "$") {
				throw runtime_error("unexpected shift by $");
			}
			++current_position;
			path.push(EdgeVertex{current_symbol, cell.number});
		} else if (cell.cell_type == reduce) {
			if (states[path.top().state_number] == GoTo(states[0], "S", grammar) &&
						current_position == static_cast<int>(s.size()) - 1) {
				return true;
			}
			for (unsigned i = 0; i < grammar.rules[cell.number].to.size(); ++i) {
				if (path.empty()) {
					return false;
				}
				if (path.top().symbol != grammar.rules[cell.number].to[
							grammar.rules[cell.number].to.size() - 1 - i]) {
					throw runtime_error("unexpected error");
				}
				path.pop();
			}
			int current_state = path.top().state_number;
			string from_symbol = grammar.rules[cell.number].from;
			LRCell current_cell = table[current_state][from_symbol];
			if (current_cell.cell_type != shift) {
				return false;
			}
			path.push({from_symbol, current_cell.number});
		}
	}
}