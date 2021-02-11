#include "interpreter.h"

#include <iostream>
#include <fstream>
#include <string>

using std::cerr;
using std::cin;
using std::cout;
using std::ifstream;
using std::string;

void runPascalProgram(const string& program_path) {
	Grammar grammar;
	ifstream fin1("../data/grammar.txt");
	fin1 >> grammar;
	ifstream fin2(program_path);
	if (!fin1.is_open() || !fin2.is_open()) {
		cerr << "failed to open files" << endl;
		return;
	}
	string s;
	while (fin2) {
		string s0;
		getline(fin2, s0);
		s0 += "\n";
		s += s0;
	}
	cout << "program text: " << endl << s;
	bool is_recognized =  isRecognized(grammar, s);
	cout << "program is " << (is_recognized ? "" : "in") << "correct" << endl << endl;
	if (!is_recognized)
		return;
	SyntaxTree tree = getSyntaxTree(grammar, s);
	Interpreter interpreter;
	interpreter.interpret(tree, grammar);
	cerr << "interpreted successfully!" << endl;
}

int main(int argc, char* argv[]) {
	string program_path = ((argc == 1) ?
		"../data/program.txt" : argv[1]);
	runPascalProgram(program_path);
	return 0;
}
