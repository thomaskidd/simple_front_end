// Simple Lexer for C like language
#include <fstream>
#include <string>

// testing
#include <iostream>

// Token types - unknown tokens are characters [0-255]
enum TokenType 
{
	// Error State
	token_error = -1,

	// Controls
	token_eof = -2,

	// Code tokens
	token_identifier = -3,
	token_number = -4
};

// Definition
class Lexer 
{

private:
	// Tokens
	std::string _lastToken;
	TokenType _lastTokenType;

	// File IO
	bool _fileIsOpen;
	std::ifstream _file;

public:
	// Constructor & Destructor
	Lexer();
	~Lexer();

	// Setters
	bool openFile(std::string filename);
	bool closeFile();

	// Getters
	std::string getLastToken();
	TokenType getLastTokenType();

	// Consume token
	bool readToken();
};

// Implementations
Lexer::Lexer() : _lastToken(""), _lastTokenType(token_error)
{
	// Do Nothing
}

Lexer::~Lexer()
{
	_lastToken = "";
	_lastTokenType = token_error;
}

// Setters
bool Lexer::openFile(std::string filename)
{
	// Error Check
	if (_fileIsOpen) {
		return false;
	}

	_file.open(filename, std::ios::in);
	_fileIsOpen = _file.is_open();

	return _fileIsOpen;
}

bool Lexer::closeFile()
{
	// Error Check
	if (!_fileIsOpen) {
		return false;
	}

	_file.close();
	_fileIsOpen = _file.is_open();

	return !(_fileIsOpen);
}

// Getters
std::string Lexer::getLastToken()
{
	return _lastToken;
}

TokenType Lexer::getLastTokenType()
{
	return _lastTokenType;
}

// Tokenizer
bool Lexer::readToken()
{
	// Error Check
	if (!_fileIsOpen) {
		return false;
	}

	// Token to create
	std::string token = "";

	// Remove leading whitespace
	_file >> std::ws;

	// Peek at next character
	char nextChar = _file.peek();

	// Identifier - alphanumeric word starting with a letter
	if (isalpha(nextChar)) {

		// Read identifier
		while(isalnum(nextChar)) {
			char curChar = ' ';
			curChar = _file.get();
			token += curChar;

			nextChar = _file.peek();
		}

		// Set _lastToken and _lastTokenType
		_lastToken = token;
		_lastTokenType = token_identifier;
	}
	// Number - decimals not supported yet
	else if (isdigit(nextChar)) {

		// Read number
		while(isdigit(nextChar)) {
			char curChar = ' ';
			curChar = _file.get();
			token += curChar;

			nextChar = _file.peek();
		}

		// Set _lastToken and _lastTokenType
		_lastToken = token;
		_lastTokenType = token_number;
	}
	// End of file
	else if (nextChar == EOF) {
		closeFile();

		// Set _lastToken and _lastTokenType
		_lastToken = "";
		_lastTokenType = token_eof;
	}
	// Other
	else {
		// Consume character
		char curChar = ' ';
		curChar = _file.get();

		// Check if this is the start of a comment
		nextChar = _file.peek();

		// Single Line comment
		if (nextChar == '/') {
			
			std::string comment;
			std::getline(_file, comment);

			// Set _lastToken and _lastTokenType
			readToken();
		}
		// Multi Line comment
		else if (nextChar == '*') {
			// Consume '*'
			curChar = _file.get();

			bool comment_over = false;
			while (!comment_over) {
				
				// Look for '*'
				nextChar = _file.peek();
				while (nextChar != '*' && nextChar != EOF) {
					curChar = _file.get();
					nextChar = _file.peek();
				}

				// Check for EOF
				if (nextChar == EOF) {
					comment_over = true;
				}
				// Check for end of comment
				else {
					// Consume '*'
					curChar = _file.get();
					nextChar = _file.peek();
					if (nextChar == '/') {
						// Consume '/'
						curChar = _file.get();
						comment_over = true;
					}
				}
			}

			// Set _lastToken and _lastTokenType
			readToken();
		}
		// Not a comment - set token
		else {

			// Set _lastToken and _lastTokenType
			_lastToken = curChar;
			_lastTokenType = (TokenType)curChar;
		}
	}

	return (_lastTokenType != token_eof && _lastTokenType != token_error);
}