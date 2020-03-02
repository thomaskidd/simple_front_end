// Datatype enum and related functions
#pragma once

#include <map>

enum DataType {
	datatype_unassigned = 0,
	datatype_int,
	datatype_float,
	datatype_char
};

static std::map<std::string, DataType> dataTypeMap =
{
	{"int", datatype_int},
	{"float", datatype_float},
	{"char", datatype_char}
};

DataType str2datatype(const std::string &str) {
	if (dataTypeMap.count(str)) {
		return dataTypeMap.at(str);
	}

	return datatype_unassigned;
}