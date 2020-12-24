#pragma once

#include "grammar.h"

Grammar removeEpsilon(const Grammar& grammar);
// this function can only remove epsilon rules after the main
// part of chomsky to greybuh algorithm

int classifyRuleChomskyToGreybuh(const Rule& rule, const string& starting_symbol);
void processRule(Grammar& grammar, const Rule& rule);
Grammar chomskyToGreybuh(const Grammar& grammar);
