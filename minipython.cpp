#include <iostream>
#include <fstream>
#include <vector>
#include <stack>

#include "tokens.h"
#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "global_scope.h"
#include "interpreter.h"
#include "error.h"
#include "DebugFuncs.h"

using namespace std;

bool inBlock = false;

int main(int argc, char *argv[]) {
	/*====file input====*/
	//check if input file is provided
	if(argc < 2) {
		cout << "minipython: no input file provided" << endl;
		return 0;
	}
	
	string inFile = argv[1];
	ifstream inputProgram(inFile);
	
	//check if file exists
	if(!inputProgram.is_open()) {
		cout << "minipython: can't open file \'" << inFile << "\', no such file in directory" << endl;
		inputProgram.close();
		return 0;
	}
	/*==end file input==*/
	
	/*====Interpreter====*/
	try {
		LexicalAnalyzer lexer;
		Parser parse;
		Interpreter interpret;
	
		string line;
		int lineCtr = 1;
		while(getline(inputProgram, line)) {
			/*====Lexical Analysis====*/
			lexer.initialize(line, lineCtr);
			lexer.tokenize();
			if(inputProgram.peek() == EOF) {
				lexer.addEndStmntTokenIfNecessary(true);
			}
			vector<Token> tokens = lexer.getTokens();
			//representTokenList(tokens); //debug function
			/*==end Lexical Analysis*/
			
			/*====Parser====*/
			ASTNode* tree = nullptr;
			
			parse.initialize(tokens);
			parse.parseAndCreateAST();
			tree = parse.getAST();
			
			//representAST(tree); cout << endl; //debug function
			/*==end Parser==*/
			
			/*====Code Interpreter====*/
			interpret.initialize(tree);
			interpret.evaluate();
			/*==end Code Interpreter==*/
			
			lineCtr++;
		}
		inputProgram.close();
	}
	
	catch(CreateProgramError& e) {
		cout << e.what() << endl;
		return -1;
	}
	/*==end Interpreter==*/
}