// Binary Operator Precedence
#pragma once

static std::map<std::string, int> binop_precedence;

binop_precedence["~"]  = 40;
binop_precedence[">>"] = 30;
binop_precedence["<<"] = 30;
binop_precedence["*"]  = 20;
binop_precedence["/"]  = 20;
binop_precedence["+"]  = 10;
binop_precedence["-"]  = 10;
binop_precedence["<"]  = 1;
binop_precedence[">"]  = 1;