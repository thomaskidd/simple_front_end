// Test Driver for Simple Front End
#include <iostream>

#include "lexer.h"
#include "parser.h"

using namespace std;

int main(int argv, char **argc) {

	Lexer lexer;
	lexer.openFile("test/example.c");

	while (lexer.readToken()) {
		cout << lexer.getLastToken() << endl;
	}
	
	return 0;
}