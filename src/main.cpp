#include "earley.h"

#include <iostream>

using std::cin;
using std::cout;
using std::endl;

void checkRecognition() {
	Grammar grammar;
	cout << "enter grammar:\n";
	cin >> grammar;
	cout << "enter string to check:";
	string s;
	cin >> s;
	cout << EarleyAlgorithm().isRecognized(grammar, s) << endl;
}

int main() {
	checkRecognition();
    return 0;
}

/*
input examples:

S'
3
S' 1 S
S 2 S a
S 1 a
aaaaaaaaaa

S'
3
S' 1 S
S 4 ( S ) S
S 0
((()())(()())())
 */
