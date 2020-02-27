// Simple Parser for C like language
#pragma once

#include "ast.h"
#include "binop.h"
#include "lexer.h"

class Parser {
private:
	std::unique_ptr<Lexer> _lexer;

	/* Token Consumption */
	TokenType peekTokenType();
	std::string peekToken();
	void consumeToken();
	std::string readAndConsumeToken();

	/* Operator Precedence */
	int getTokenPrec(std::string &token);

	/* Internal Parsing Methods */

	// Integers, Floats, Chars - TODO: Support float, char
	std::unique_ptr<ExprAST> parseLiteral();

	// Variables, Function Calls
	std::unique_ptr<ExprAST> parseIdentifier();

	// Expression enclosed in ( )
	std::unique_ptr<ExprAST> parseParenthesis();

	// Handler for Literal, Identifier, Parenthesis expressions
	std::unique_ptr<ExprAST> parsePrimary();

	// Right hand side of binary expressions
	std::unique_ptr<ExprAST> parseBinOpRHS(int leftPrec, std::unique_ptr<ExprAST>left);

	// Handler for all expressions
	std::unique_ptr<ExprAST> parseExpression();

	// Function Prototypes
	std::unique_ptr<ExprAST> parsePrototype();

	// Function Definitions
	std::unique_ptr<ExprAST> parseFunction();


public:
	Parser(std::unique_ptr<Lexer> lexer);
	~Parser();

	// Top Level Parser
	std::unique_ptr<ExprAST> parseTopLevel();
};

// Implementation

// Public
Parser::Parser(std::unique_ptr<Lexer> lexer) : _lexer(std::move(lexer)) {
	// Nothing to do
}

Parser::~Parser() {
	// Nothing to do
}

std::unique_ptr<ExprAST> Parser::parseTopLevel() {
	// Implement later
}

// Private

/* Helper Methods */

TokenType Parser::peekTokenType() {
	return _lexer->getLastTokenType();
}

std::string Parser::peekToken() {
	return _lexer->getLastToken();
}

void Parser::consumeToken() {
	_lexer->readToken();;
	return;
}

// Return and consume the most recent token
std::string Parser::readAndConsumeToken() {
	auto lastToken = _lexer->getLastToken();
	_lexer->readToken();

	return lastToken;
}

// Return -1 for an invalid token
getTokenPrec(std::string &token) {
	if (binop_precedence.count(token) != 0) {
		return binop_precedence.at(token);
	}
	else {
		return -1;
	}
}

/* Parsing Methods */

// Integers, Floats, Chars - TODO: Support float, char
std::unique_ptr<ExprAST> Parser::parseLiteral() {
	std::string token = readAndConsumeToken();
	auto literal = std::make_unique<IntegerAST>(std::stoi(token));
	return std::move(literal);
}

// Variables, Function Calls
std::unique_ptr<ExprAST> Parser::parseIdentifier() {
	std::string token = readAndConsumeToken();

	std::unique_ptr<ExprAST> id;

	// Check if this is a function call
	if (peekToken() == "(") {
		consumeToken();

		// Args
		std::vector<std::unique_ptr<ExprAST>> callArgs;

		while (true) {
			// Add expression to callArgs vector
			if (auto arg = parseExpression()) {
				callArgs.push_back(std::move(arg));
			}
			else {
				return nullptr;
			}

			// Break if ")" is reached
			if (peekToken() == ")") {
				break;
			}

			// Else consume ","
			if (peekToken() != ",") {
				return nullptr;
			}
			consumeToken();
		}

		// Consume ")"
		consumeToken();

		// Data type will for calls will be determined during linking time
		id = std::make_unique<CallAST>(datatype_unassigned, token, std::move(callArgs));
	}
	else {
		// Variable will have to be looked up to determine its type
		id = std::make_unique<VariableAST>(datatype_unassigned, token);
	}

	return std::move(id);
}

// Expression enclosed in ( )
std::unique_ptr<ExprAST> Parser::parseParenthesis() {
	// Error Check
	if (peekToken() != "(") {
		return nullptr;
	}
	consumeToken();

	// Parse internal expresssion
	auto expr = parseExpression();

	// Error Check
	if (peekToken() != ")") {
		return nullptr;
	}
	consumeToken();

	return expr;
}

// Handler for Literal, Identifier, Parenthesis expressions
std::unique_ptr<ExprAST> Parser::parsePrimary() {
	switch(peekTokenType()) {

		case token_literal:
			return parseLiteral();

		case token_identifier:
			return parseIdentifier();

		case '(':
			return parseParenthesis();

		// Error
		default:
			return nullptr;
	}
}

// Right hand side of binary expressions
std::unique_ptr<ExprAST> Parser::parseBinOpRHS(int leftPrec, std::unique_ptr<ExprAST>left) {

}

// Handler for all expressions
std::unique_ptr<ExprAST> Parser::parseExpression() {
	auto left = parsePrimary();
	if (left == nullptr) {
		return nullptr;
	}

	retrun parseBinOpRHS(0, std::move(left));
}

// Function Prototypes
std::unique_ptr<ExprAST> Parser::parsePrototype() {

}

// Function Definitions
std::unique_ptr<ExprAST> Parser::parseFunction() {

}