#include "solution.h"
#include <iostream>

using std::cin;
using std::cout;

void solveProblem() {
    Grammar grammar;
    cin >> grammar; // grammar has to be in normal Chomsky form
    grammar = chomskyToGreybuh(grammar);
    cout << grammar;
}

int main() {
    solveProblem();
    return 0;
}

/*
input for task 3, HW 6:

S'
16
S' 2 B T
S' 2 A S
S 2 B T
S 2 A S
T 2 U C
T 2 C T
U 2 B U1
U 2 C U2
U1 2 V A
U2 2 U C
V 2 V V1
V 1 b
V1 2 A B
A 1 a
B 1 b
C 1 c
*/
