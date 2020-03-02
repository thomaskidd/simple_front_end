// Binary Operator Precedence
#pragma once

#include <map>
#include <string>

static std::map<std::string, int> binop_precedence =
{
	{"~",  40},
	{">>", 30},
	{"<<", 30},
	{"*",  20},
	{"/",  20},
	{"+",  10},
	{"-",  10},
	{"<",  1},
	{">",  1}
};