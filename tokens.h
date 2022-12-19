#ifndef TOKENS_H
#define TOKENS_H

#include <iostream>
using namespace std;

enum TokenType {
	T_Identifier, T_String_Literal, T_INT,
	T_Keyword,
	T_SEPARATOR, T_OpenParen, T_CloseParen, T_OpenBracket, T_CloseBracket,
	T_Comma, T_Colon, T_StatementEnd, T_EndLine, T_EOF,
	T_OPERATOR, T_Plus, T_Minus, T_Mult, T_Div, T_Greater, T_Less, T_EQ,
	T_NONE
};

struct Token {
	TokenType token_type;
	string token_value;
	int tok_lineNum;
	int tok_pos;
	
	Token() {}
	Token(TokenType t_type, string t_val, int inLineNum, int inPos) {
		token_type = t_type;
		token_value = t_val;
		tok_lineNum = inLineNum;
		tok_pos = inPos;
	}
	
	void TokenCopy(Token t) {
		token_type = t.token_type;
		token_value = t.token_value;
		tok_lineNum = t.tok_lineNum;
		tok_pos = t.tok_pos;
	}
};

#endif