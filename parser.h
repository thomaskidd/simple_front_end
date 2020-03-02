// Simple Parser for C like language
#pragma once

#include <string>
#include <vector>

#include "ast.h"
#include "datatype.h"
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
	int getTokenPrec(const std::string &token);

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
	std::unique_ptr<PrototypeAST> parsePrototype();

	// Function Definitions
	std::unique_ptr<FunctionAST> parseFunction();


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
int Parser::getTokenPrec(const std::string &token) {
	if (binop_precedence.count(token)) {
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

	while(true) {

		std::string binop = peekToken();
		int binopPrec = getTokenPrec(binop);

		// If next operation has a lower precedence
		// do not bind it to this binop
		if (binopPrec < leftPrec) {
			return left;
		}

		// Consume binop token
		consumeToken();

		// Create the expression on the other side of the binop
		auto right = parsePrimary();
		if (!right) {
			return nullptr;
		}

		// If the next binop binds tighter, add it to the
		// right hand side
		std::string nextBinop = peekToken();
		int nextBinopPrec = getTokenPrec(nextBinop);

		if (nextBinopPrec > binopPrec) {

			// Everything with a higher precedence than binopPrec should be
			// included in the right hand side
			right = parseBinOpRHS(binopPrec + 1, std::move(right));
		}


		// Re-assign new left hand side
		left = std::make_unique<BinaryExprAST>(left->getDataType(), binop, std::move(left), std::move(right));
	}
}

// Handler for all expressions
std::unique_ptr<ExprAST> Parser::parseExpression() {
	auto left = parsePrimary();
	if (!left) {
		return nullptr;
	}

	return parseBinOpRHS(0, std::move(left));
}

// Function Prototypes
std::unique_ptr<PrototypeAST> Parser::parsePrototype() {

	// Get return type
	if (peekTokenType() != token_identifier) {
		return nullptr;
	}
	DataType returnType = str2datatype(readAndConsumeToken());

	// Get name
	if (peekTokenType() != token_identifier) {
		return nullptr;
	}
	std::string name = readAndConsumeToken();

	if (peekToken() != "(") {
		return nullptr;
	}
	// Consume "("
	consumeToken();
	std::vector<std::string> params;
	std::vector<DataType> paramTypes;

	if (peekToken() != ")") {

		while(true) {

			// TODO - add error checking
			DataType type = str2datatype(readAndConsumeToken());
			std::string param = readAndConsumeToken();

			paramTypes.push_back(type);
			params.push_back(param);

			// Check for end of prototype
			if (peekToken() == ")") {
				break;
			}

			// Handle comma
			if (peekToken() != ",") {
				return nullptr;
			}
			// Consume ","
			consumeToken();
		}

	}
	// Consume ")"
	consumeToken();

	return std::make_unique<PrototypeAST>(returnType, name, std::move(paramTypes), std::move(params));
}

// Function Definitions
std::unique_ptr<FunctionAST> Parser::parseFunction() {
	auto prototype = parsePrototype();
	if (!prototype) {
		return nullptr;
	}

	// auto expr = 
}