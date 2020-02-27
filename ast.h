// Simple Abstract Syntax Tree nodes for C like language
#pragma once

#include <memory>
#include <string>
#include <vector>

enum DataType {
	datatype_unassigned = 0,
	datatype_int,
	datatype_float,
	datatype_char
};

// Expression Base Class
class ExprAST {
protected:
	DataType _dataType;

public:
	ExprAST(DataType dataType) : _dataType(dataType) {}
	virtual ~ExprAST() {}
};

/* Literals */

// Integer Literals
class IntegerAST : public ExprAST {
private:
	int _val;

public:
	IntegerAST(int val) : ExprAST(datatype_int), _val(val) {}
};

// Floating Point Literals - TODO: Support this dataype
class FloatAST : public ExprAST {
private:
	float _val;

public:
	FloatAST(float val) : ExprAST(datatype_float), _val(val) {}
};

// Character Literals - TODO: Support this dataype
class CharAST : public ExprAST {
private:
	char _val;

public:
	CharAST(char val) : ExprAST(datatype_char), _val(val) {}
};

/* Variables */

// Variables
class VariableAST : public ExprAST {
private:
	std::string _name;

public:
	VariableAST(DataType type, const std::string &name) : ExprAST(type), _name(name) {}
};

/* Binary Expressions */

// Binary Expressions
class BinaryExprAST : public ExprAST {
private:
	char _binop;
	std::unique_ptr<ExprAST> _leftExpr;
	std::unique_ptr<ExprAST> _rightExpr;

public:
	BinaryExprAST(DataType type, char binop, std::unique_ptr<ExprAST> left, std::unique_ptr<ExprAST> right) :
	ExprAST(type),
	_binop(binop),
	_leftExpr(std::move(left)),
	_rightExpr(std::move(right))
	{}
};

/* Functions */

// Function Protoypes
class PrototypeAST {
private:
	std::string _name;
	std::vector<std::string> _params;
	std::vector<DataType> _paramTypes;
	DataType _returnType;

public:
	PrototypeAST(const std::string &name, 
		std::vector<std::string> params, 
		std::vector<DataType> paramTypes,
		DataType returnType) :
	_name(name),
	_params(params),
	_paramTypes(paramTypes),
	_returnType(returnType)
	{}
};

// Functions
class FunctionAST {
private:
	std::unique_ptr<PrototypeAST> _prototype;
	std::vector<std::unique_ptr<ExprAST>> _exprs;

public:
	FunctionAST(std::unique_ptr<PrototypeAST> prototype,
		std::vector<std::unique_ptr<ExprAST>> exprs) :
	_prototype(std::move(prototype)),
	_exprs(std::move(exprs))
	{}
};

// Function Calls
class CallAST : public ExprAST {
private:
	std::string _func;
	std::vector<std::unique_ptr<ExprAST>> _args;

public:
	CallAST(DataType type, const std::string &func, std::vector<std::unique_ptr<ExprAST>> args) :
	ExprAST(type),
	_func(func),
	_args(std::move(args))
	{}
};

// TODO Implement Branching

/* Looping and Branching */

// Branching
class BranchAST {
private:
	std::unique_ptr<ExprAST> _condition;

};

// While Loops