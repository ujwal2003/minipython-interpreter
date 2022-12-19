#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include "tokens.h"
#include "error.h"

using namespace std;

class LexicalAnalyzer {
	private:
		string line;
		int currPos;
		char currChar;
		int lineNumber;
		vector<Token> tokens;
		stack<int> keywordPositions;
		
		void incrementChar() {
			currPos += 1;
			currChar = (currPos < line.size()) ? line[currPos] : '\0';
		}
		
	public:
		void initialize(string currLine, int currLineNum) {
			line = currLine;
			currPos = -1;
			currChar = '\0';
			lineNumber = currLineNum;
			tokens.clear();
			incrementChar();
		}
		
		vector<Token> getTokens() {
			return tokens;
		}
		
		void tokenize() {
			while(currChar != '\0') {
				//comments
				if(currChar == '#') {
					addEndStmntTokenIfNecessary();
					while(currChar != '\0')
						incrementChar();
					continue;
				}
				//empty space
				else if(isblank(currChar)) {
					goToNotEmpty();
					addEndStmntTokenIfNecessary();
					continue;
				}
				//operator
				else if(currChar == '+' || currChar == '-' || currChar == '*' || currChar == '/' || 
						currChar == '=' || currChar == '>' || currChar == '<') {
					addEndStmntTokenIfNecessary();
					tokens.push_back(getOperatorToken(currChar));
					incrementChar();
					continue;
				}
				//separator
				else if(currChar == '(' || currChar == ')' || currChar == '[' || currChar == ']' ||
						currChar == ',' || currChar == ':') {
					addEndStmntTokenIfNecessary();
					tokens.push_back(getSeparatorToken(currChar));
					incrementChar();
					continue;
				}
				//integer
				else if(isdigit(currChar)) {
					addEndStmntTokenIfNecessary();
					tokens.push_back(Token(T_INT, makeInteger(), lineNumber, currPos));
					continue;
				}
				//string literal
				else if(currChar == '\"') {
					addEndStmntTokenIfNecessary();
					incrementChar();
					string tk = makeStringLiteral();
					tokens.push_back(Token(T_String_Literal, tk, lineNumber, currPos));
					incrementChar();
					continue;
				}
				//identifiers and keywords
				else if(isalpha(currChar)) {
					addEndStmntTokenIfNecessary();
					int firstPos = currPos;
					string tk = makeLiteral();
					if(isKeyword(tk)) {
						tokens.push_back(Token(T_Keyword, tk, lineNumber, currPos));
						if(tk == "if" || tk == "else" || tk == "def" || tk == "while") {
							keywordPositions.push(firstPos);
						}
					} else {
						tokens.push_back(Token(T_Identifier, tk, lineNumber, currPos));
					}
					continue;
				}
				//error
				else {
					RaiseError(InvalidCharacterError, string(1,currChar), lineNumber);
				}
			}
			//marks the end of the line
			tokens.push_back(Token(T_EndLine, "", lineNumber, -1));
		}
		
		//goes to the next non-empty char
		void goToNotEmpty() {
			while(currChar != '\0' && isblank(currChar)) {
				incrementChar();
			}
		}
		//returns operator token
		Token getOperatorToken(char c) {
			switch(c) {
				case '+': return Token(T_Plus, string(1,c), lineNumber, currPos);
				case '-': return Token(T_Minus, string(1,c), lineNumber, currPos);
				case '*': return Token(T_Mult, string(1,c), lineNumber, currPos);
				case '/': return Token(T_Div, string(1,c), lineNumber, currPos);
				case '=': return Token(T_EQ, string(1,c), lineNumber, currPos);
				case '>': return Token(T_Greater, string(1,c), lineNumber, currPos);
				case '<': return Token(T_Less, string(1,c), lineNumber, currPos);
				default: return Token(T_OPERATOR, string(1,c), lineNumber, currPos);
			}
		}
		//returns separator token
		Token getSeparatorToken(char c) {
			switch(c) {
				case '(': return Token(T_OpenParen, string(1,c), lineNumber, currPos);
				case ')': return Token(T_CloseParen, string(1,c), lineNumber, currPos);
				case '[': return Token(T_OpenBracket, string(1,c), lineNumber, currPos);
				case ']': return Token(T_CloseBracket, string(1,c), lineNumber, currPos);
				case ',': return Token(T_Comma, string(1,c), lineNumber, currPos);
				case ':': return Token(T_Colon, string(1,c), lineNumber, currPos);
				default: return Token(T_SEPARATOR, string(1,c), lineNumber, currPos);
			}
		}
		//returns the integer as a string
		string makeInteger() {
			string int_str = "";
			while(currChar != '\0' && isdigit(currChar)) {
				int_str += currChar;
				incrementChar();
			}
			return int_str;
		}
		//returns the string literal as a string
		string makeStringLiteral() {
			string lit = "";
			while(currChar != '\0' && currChar != '\"') {
				lit += currChar;
				incrementChar();
			}
			//if no end quote found
			if(currChar != '\"'){
				//TODO: make error class and invoke here
				// cout << "Error: Expected \" at line " << lineNumber << endl;
				// exit(0);
				RaiseError(DefaultError, string(1,currChar), lineNumber);
			}
			return lit;
		}
		//returns identifier or keyword as a string
		string makeLiteral() {
			string lit = "";
			while(currChar != '\0' && isalnum(currChar)) {
				lit += currChar;
				incrementChar();
			}
			return lit;
		}
		//returns true if given string is a keyword
		bool isKeyword(string str) {
			if(str == "if" || str == "else" || str == "def" || str == "while" || str == "return" || str == "print" || str == "len") {
				return true;
			}
			return false;
		}
		//adds a end statement token if the if/else/while/def block has ended
		void addEndStmntTokenIfNecessary(bool eofFlag=false) {
			if(eofFlag) {
				while(!keywordPositions.empty()) {
					tokens.push_back(Token(T_StatementEnd, "", lineNumber, currPos));
					tokens.push_back(Token(T_EndLine, "", lineNumber, currPos));
					keywordPositions.pop();
				}
				return;
			}
			
			if(keywordPositions.size() > 0 && currPos != -1) {
				if(currPos == 0 || currPos == keywordPositions.top() || currPos < keywordPositions.top()) {
					tokens.push_back(Token(T_StatementEnd, "", lineNumber, currPos));
					tokens.push_back(Token(T_EndLine, "", lineNumber, currPos));
					keywordPositions.pop();
				}
			}
			
			while(!keywordPositions.empty()) {
				if(currPos == 0 || currPos == keywordPositions.top() || currPos < keywordPositions.top()) {
					tokens.push_back(Token(T_StatementEnd, "", lineNumber, currPos));
					tokens.push_back(Token(T_EndLine, "", lineNumber, currPos));
					keywordPositions.pop();
				} else {
					break;
				}
			}
		}
};

#endif