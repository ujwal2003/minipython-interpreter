#ifndef GLOBAL_SCOPE_H
#define GLOBAL_SCOPE_H

#include <iostream>
#include <map>
#include <utility>
#include <stack>
#include "ast.h"

using namespace std;

struct evalHolder {
	DataType dat;
	string val;
	vector<string> listVal;
};

//global
stack<evalHolder> evalTracker;
map<string, pair<string, DataType>> symbolTable; //name, <value, dataType>
map<string, vector<string>> listSymbolTable; //name, vector<string>

#endif