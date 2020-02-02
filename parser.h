// Simple Parser for C like language
#pragma once

#include "ast.h"
#include "lexer.h"

class Parser {
	std::unique_ptr<Lexer> _lexer;

public:
	Parser(std::unique_ptr<Lexer> lexer) : _lexer(std::move(lexer)) {}
};