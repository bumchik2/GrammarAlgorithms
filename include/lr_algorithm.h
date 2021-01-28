#include "grammar.h"
#include "syntax_tree.h"

#include <unordered_map>
#include <unordered_set>

using std::unordered_map;
using std::unordered_set;

class LRSituation {
public:
	LRSituation(const Rule& rule, int position_in_rule):
		rule(rule), position_in_rule(position_in_rule) {}
	Rule rule;
	int position_in_rule = -1;
};

bool operator == (const LRSituation&, const LRSituation&);
ostream& operator << (ostream&, const LRSituation&);

class LRSituationHash {
public:
	size_t operator() (const LRSituation& lr_situation) const {
		size_t hash1 = RuleHash()(lr_situation.rule);
		size_t hash2 = std::hash<int>()(lr_situation.position_in_rule);
		return hash1 ^ (hash2 << 1);
	}
};

class LRState {
public:
	LRState() = default;
	LRState(const unordered_set<LRSituation, LRSituationHash>& situations):
			situations(situations) {}
	unordered_set<LRSituation, LRSituationHash> situations;
};

ostream& operator << (ostream& os, const LRState&);
bool operator == (const LRState&, const LRState&);

enum LRCellType {
	shift,
	reduce,
	error
};

const unordered_map<LRCellType, string> LRCellTypeToString = {
	{shift, "shift"},
	{reduce, "reduce"},
	{error, "error"},
};

struct LRCell {
	LRCellType cell_type;
	int number; // state_number or rule_number
};

class LRAlgorithm {
public:
	LRAlgorithm(const Grammar& grammar): grammar(grammar) {
		this->grammar.addAlphabetSymbol("$");
		createTable();
	}
	bool isRecognized(string);
	void printTable() const;
public:
	LRCell getCell(int state_number, const string& symbol) const;
	void createTable();
	void LRDFS(int state_number);
	Grammar grammar;
	vector<unordered_map<string, LRCell>> table;
	vector<LRState> states;
};

vector<LRSituation> LRPredict(const LRSituation&, const Grammar&);
LRState Closure(LRState, const Grammar& grammar);
LRState GoTo(const LRState&, const string& symbol, const Grammar& grammar);
