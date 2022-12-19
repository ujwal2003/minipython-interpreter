#ifndef DEBUGFUNCS_H
#define DEBUGFUNCS_H

#include <iostream>
#include <vector>
#include "tokens.h"
#include "ast.h"

using namespace std;

string stringVector(vector<string> v) {
	string str;
	for(int i=0; i<v.size(); i++) {
		if(i != 0)
			str += ", ";
		str += v[i];
	}
	return str;
}

string representToken(Token t) {
	switch(t.token_type) {
		case T_Identifier: return "IDENTIFIER: " + t.token_value;
		case T_String_Literal: return "STR_LITERAL: " + t.token_value;
		case T_INT: return "INT: " + t.token_value;
		case T_Keyword: return "KEYWORD: " + t.token_value + " (pos: " + to_string(t.tok_pos) + ")";
		case T_SEPARATOR: return "unknown separator: " + t.token_value;
		case T_OpenParen: return "OPEN_PAREN";
		case T_CloseParen: return "CLOSED_PAREN";
		case T_OpenBracket: return "OPEN_BRACKET";
		case T_CloseBracket: return "CLOSED_BRACKET";
		case T_Comma: return "COMMA";
		case T_Colon: return "COLON";
		case T_StatementEnd: return "STMT_END" + t.token_value + " (pos: " + to_string(t.tok_pos) + ")";
		case T_EndLine: return "ENDLINE";
		case T_EOF: return "EOF";
		case T_OPERATOR: return "unknown operator: " + t.token_value;
		case T_Plus: return "PLUS";
		case T_Minus: return "MINUS";
		case T_Mult: return "MULT";
		case T_Div: return "DIV";
		case T_Greater: return "GREATER THAN";
		case T_Less: return "LESS THAN";
		case T_EQ: return "EQUALS";
		case T_NONE: return "NONE";
		default: return "UNKOWN TOKEN: " + t.token_value;
	}
}

void representTokenList(vector<Token> &tokenList) {
	for(Token t: tokenList) {
		cout << "{ " << representToken(t) << " } ";
		if(t.token_type == T_EndLine)
			cout << endl;
	}
}

string representDataType(DataType d) {
	switch(d) {
		case INT: return "INT";
		case LIST: return "LIST";
		case STR_LITERAL: return "STR_LITERAL";
		case D_NIL: return "NIL";
		default: return "NIL";
	}
}

void representAST(ASTNode* root) {
	//NULL
	if(root == nullptr) {
		cout << "NULL";
		return;
	}
	//number node
	if(root->type == N_Number) {
		cout << "NUMBER:{" << root->nodeVal << "}";
		return;
	}
	//string literal node
	if(root->type == N_StrLtr) {
		cout << "STR_LITERAL:{" << root->nodeVal << "}";
		return;
	}
	//list node
	if(root->type == N_List) {
		string str = stringVector(root->listVal);
		cout << "LIST:{" << str << "}";
		return;
	}
	//list access node
	if(root->type == N_ListAcc) {
		cout << "ACCESS:{";
		representAST(root->left);
		cout << '[';
		representAST(root->right);
		cout << "]}";
		return;
	}
	//list splice node
	if(root->type == N_List_Splice) {
		cout << "SPLICE:{";
		representAST(root->left);
		cout << " at ";
		representAST(root->right);
		cout << '}';
		return;
	}
	//var node
	if(root->type == N_Var) {
		cout << "VAR:{" << "NAME:(" << root->nodeVal << ") TYPE:(" << representDataType(root->dataType) << ") VALUE:(";
		representAST(root->child);
		cout << ")}";
		return;
	}
	//assign node
	if(root->type == N_Assign) {
		cout << "ASSIGN:{";
		representAST(root->left);
		cout << " = ";
		representAST(root->right);
		cout << '}';
		return;
	}
	//plus node
	if(root->type == N_Plus) {
		cout << "ADD:{";
		representAST(root->left);
		cout << " + ";
		representAST(root->right);
		cout << '}';
		return;
	}
	//print(one_arg)
	if(root->type == N_Print1) {
		cout << "PRINT:{";
		representAST(root->child);
		cout << '}';
		return;
	}
	//print(two_args)
	if(root->type == N_Print2) {
		cout << "PRINT:{";
		representAST(root->left);
		cout << " , ";
		representAST(root->right);
		return;
	}
}

#endif