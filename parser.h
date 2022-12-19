#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include <map>
#include <utility>
#include "tokens.h"
#include "ast.h"
#include "global_scope.h"
#include "error.h"
#include "DebugFuncs.h"

using namespace std;

class Parser {
	private:
		vector<Token> tokens;
		int tok_idx;
		Token currTok;
		ASTNode* tree = nullptr;
		
		//advances to next token
		void nextToken() {
			tok_idx += 1;
			if(tok_idx < tokens.size()) {
				currTok.TokenCopy(tokens[tok_idx]);
			} else {
				currTok.TokenCopy(Token(T_NONE, "", -1, -1));
			}
		}
		//sets currTok to token at specific index
		void setCurrTokIndex(int i) {
			if(i < tokens.size()) {
				tok_idx = i;
				currTok.TokenCopy(tokens[tok_idx]);
			} else {
				currTok.TokenCopy(Token(T_NONE, "", -1, -1));
			}
		}
		//raise syntax error
		void raiseSyntaxError(string txt, int lineNumber, ErrType errTypeOverride=InvalidSyntaxError) {
			deleteAST(tree);
			//cout << "AT ERROR: current token is " << representToken(currTok) << " at token pos " << tok_idx << endl;
			RaiseError(errTypeOverride, txt, lineNumber);
		}
		
		//modified shunting yard algorithm
		queue<ASTNode*> shuntingYardAlgo(vector<ASTNode*> inNodeList) {
			stack<ASTNode*> operatorStack;
			queue<ASTNode*> outputQueue;
			
			for(ASTNode* node: inNodeList) {
				if(node->type == N_Var || node->type == N_Number || node->type == N_ListAcc) {
					outputQueue.push(node);
				}
				
				if(node->type == N_Plus) {
					operatorStack.push(node);
				}
			}
			
			while(!operatorStack.empty()) {
				outputQueue.push(operatorStack.top());
				operatorStack.pop();
			}
			
			return outputQueue;
		}
		
	public:
		//initialization
		void initialize(vector<Token> inTokenList) {
			//deleteAST(tree);
			tokens = inTokenList;
			tok_idx = -1;
			nextToken();
		}
		
		//syntax analysis and AST creation
		void parseAndCreateAST() {
			//empty or comment
			if(currTok.token_type == T_EndLine && tokens.size() == 1) {
				tree = nullptr;
				nextToken();
			}
			
			//Identifier; check for assignment
			if(currTok.token_type == T_Identifier) {
				tree = assign(tok_idx);
				
				if(tree->type == N_NILNode) {
					raiseSyntaxError("different syntax for identifier", currTok.tok_lineNum);
				}
				
				nextToken();
			}
			
			//TODO: keyword; check for print
			if(currTok.token_type == T_Keyword) {
				//print
				if(currTok.token_value == "print") {
					tree = printOneOrTwo(tok_idx);
					
					if(tree->type == N_NILNode) {
						raiseSyntaxError("different syntax for print()", currTok.tok_lineNum);
					}
				
					nextToken();
				}
				
				//error
				else {
					raiseSyntaxError("different syntax for keyword " + currTok.token_value, currTok.tok_lineNum);
				}
			}
			
			//error
			if(currTok.token_type != T_NONE) {
				raiseSyntaxError("different syntax", currTok.tok_lineNum);
			}
		}
		
		ASTNode* getAST() {
			return tree;
		}
		
		//function for atom ::= INT|IDENTIFIER
		ASTNode* atom(int backTrackIdx) {
			if(currTok.token_type == T_INT) {
				ASTNode* numNode_ast = new ASTNode(N_Number, currTok.tok_lineNum);
				numNode_ast->init_numNode(currTok.token_value);
				nextToken();
				return numNode_ast;
			}
			
			if(currTok.token_type == T_Identifier) {
				ASTNode* varNode_ast = new ASTNode(N_Var, currTok.tok_lineNum);
				varNode_ast->init_varNode(currTok.token_value, D_NIL, nullptr);
				nextToken();
				if(currTok.token_type == T_OpenBracket) {
					delete varNode_ast;
					ASTNode* failNode_ast = new ASTNode(N_NILNode);
					setCurrTokIndex(backTrackIdx);
					return failNode_ast;
				}
				return varNode_ast;
			}
			
			ASTNode* failNode_ast = new ASTNode(N_NILNode);
			setCurrTokIndex(backTrackIdx);
			return failNode_ast;
		}
		
		//function for list_access ::= IDENTIFIER "[" atom "]"
		ASTNode* list_acc(int backTrackIdx) {
			if(currTok.token_type == T_Identifier) {
				int idtIdx = tok_idx; //store index of list var
				nextToken(); //should be open bracket
				if(currTok.token_type == T_OpenBracket) {
					nextToken(); //should be atom
					ASTNode* listIndex_ast = atom(tok_idx);
					
					//if atom fails, raise error
					if(listIndex_ast->type == N_NILNode) {
						delete listIndex_ast;
						//raiseSyntaxError("either int or identifier", currTok.tok_lineNum);
						ASTNode* failNode_ast = new ASTNode(N_NILNode);
						setCurrTokIndex(backTrackIdx);
						return failNode_ast;
					}
					
					//nextToken(); //should be closed bracket
					if(currTok.token_type == T_CloseBracket) {
						ASTNode* lstVar_ast = new ASTNode(N_Var, currTok.tok_lineNum);
						Token temp = tokens[idtIdx];
						lstVar_ast->init_varNode(temp.token_value, LIST, nullptr);
						
						ASTNode* lstAcc_ast = new ASTNode(N_ListAcc, currTok.tok_lineNum);
						lstAcc_ast->init_listAccessNode(lstVar_ast, listIndex_ast);
						nextToken();
						return lstAcc_ast;
					} else {
						//raise error
						delete listIndex_ast;
						//raiseSyntaxError("']'", currTok.tok_lineNum);
						ASTNode* failNode_ast = new ASTNode(N_NILNode);
						setCurrTokIndex(backTrackIdx);
						return failNode_ast;
					}
					
				} else {
					//not list access
					ASTNode* failNode_ast = new ASTNode(N_NILNode);
					setCurrTokIndex(backTrackIdx);
					return failNode_ast;
				}
			}
			
			//not list access
			ASTNode* failNode_ast = new ASTNode(N_NILNode);
			setCurrTokIndex(backTrackIdx);
			return failNode_ast;
		}
		
		//function for list spluce ::= IDENTIFIER "[" atom^ ":" "]" 
		ASTNode* list_splice(int backTrackIdx) {
			if(currTok.token_type == T_Identifier) {
				int idtIdx = tok_idx;
				nextToken(); //should be open bracket
				if(currTok.token_type == T_OpenBracket) {
					nextToken(); //should be either (INT|IDENTIFIER) or ":"
					
					//if ":"
					if(currTok.token_type == T_Colon) {
						nextToken(); //should be close brackets
						if(currTok.token_type == T_CloseBracket) {
							Token temp = tokens[idtIdx];
							ASTNode* lstVarNode_ast = new ASTNode(N_Var, currTok.tok_lineNum);
							lstVarNode_ast->init_varNode(temp.token_value, LIST, nullptr);
							
							ASTNode* lstSpliceNode_ast = new ASTNode(N_List_Splice, currTok.tok_lineNum);
							lstSpliceNode_ast->init_listSpliceNode(lstVarNode_ast, nullptr, "F");
							
							nextToken();
							return lstSpliceNode_ast;
						} else {
							//raise error
							raiseSyntaxError("']'", currTok.tok_lineNum);
						}
					}
					
					//if (INT|IDENTIFIER)
					if(currTok.token_type == T_INT || currTok.token_type == T_Identifier) {
						ASTNode* spliceValNode_ast = nullptr;
						if(currTok.token_type == T_INT) {
							spliceValNode_ast = new ASTNode(N_Number, currTok.tok_lineNum);
							spliceValNode_ast->init_numNode(currTok.token_value);
						} else if(currTok.token_type == T_Identifier) {
							spliceValNode_ast = new ASTNode(N_Var, currTok.tok_lineNum);
							spliceValNode_ast->init_varNode(currTok.token_value, D_NIL, nullptr);
						}
						
						nextToken(); //should be ":"
						if(currTok.token_type == T_Colon) {
							nextToken(); //should be "]"
							if(currTok.token_type == T_CloseBracket) {
								Token temp = tokens[idtIdx];
								ASTNode* lstVarNode_ast = new ASTNode(N_Var, currTok.tok_lineNum);
								lstVarNode_ast->init_varNode(temp.token_value, LIST, nullptr);
								
								ASTNode* lstSpliceNode_ast = new ASTNode(N_List_Splice, currTok.tok_lineNum);
								lstSpliceNode_ast->init_listSpliceNode(lstVarNode_ast, spliceValNode_ast, "T");
								
								nextToken();
								return lstSpliceNode_ast;
							} else {
								//raise error
								raiseSyntaxError("']'", currTok.tok_lineNum);
							}
						} 
					}
				}
			}
			
			ASTNode* failNode_ast = new ASTNode(N_NILNode);
			setCurrTokIndex(backTrackIdx);
			return failNode_ast;
		}
		
		//fucntion for expression ::= (atom|list_acc) ("+" (atom|list_acc))*
		ASTNode* expr(int backTrackIdx) {
			vector<ASTNode*> nodeExprLst;
			ASTNode* resExpr_ast = nullptr;
			ASTNode* leftOp_ast = atom(tok_idx);
			
			//check if not atom
			if(leftOp_ast->type == N_NILNode) {
				delete leftOp_ast;
				leftOp_ast = list_acc(tok_idx);
			}
			
			//if also not list access raise error
			if(leftOp_ast->type == N_NILNode) {
				delete resExpr_ast;
				delete leftOp_ast;
				raiseSyntaxError("either integer, identifier, or list access", currTok.tok_lineNum);
			}
			
			//if only single assignment
			if(currTok.token_type == T_EndLine) {
				setCurrTokIndex(tok_idx-1);
				nextToken();
				return leftOp_ast;
			}
			
			//expression
			nodeExprLst.push_back(leftOp_ast);
			if(currTok.token_type == T_Plus) {
				int i = 1;
				while(currTok.token_type == T_Plus && currTok.token_type != T_NONE) {
					//create empty (uninitialized) plus node
					nodeExprLst.push_back(new ASTNode(N_Plus, currTok.tok_lineNum));
					nextToken();
					i+=1;
					
					//get right node (atom|list_acc)
					nodeExprLst.push_back(atom(tok_idx));
					//if not atom
					if(nodeExprLst[i]->type == N_NILNode) {
						delete nodeExprLst[i];
						nodeExprLst.pop_back();
						nodeExprLst.push_back(list_acc(tok_idx));
					}
					
					//if also not list_acc; raise error
					if(nodeExprLst[i]->type == N_NILNode) {
						delete resExpr_ast;
						delete leftOp_ast;
						delete nodeExprLst[i];
						nodeExprLst.pop_back();
					} else {
						i+=1;
						continue;
					}
					
					//add both to nodeExprLst
					i+=1;
					nextToken();
				}
			} else {
				//raise error
				delete resExpr_ast;
				delete leftOp_ast;
				raiseSyntaxError("'+'", currTok.tok_lineNum);
			}
			
			//check if very last element of list isn't a plus, if plus -> raise error
			if(nodeExprLst[nodeExprLst.size()-1]->type == N_Plus) {
				for(ASTNode* n: nodeExprLst)
					delete n;
				delete resExpr_ast;
				delete leftOp_ast;
				raiseSyntaxError("either integer, identifier, or list access", currTok.tok_lineNum);
			}
			
			//apply modified shunting yard algorithm
			queue<ASTNode*> postfix_ast = shuntingYardAlgo(nodeExprLst);
			
			//apply postfix evaluation (based on image)
			queue<ASTNode*> treeQueue;
			stack<ASTNode*> operandStack;
			while(!postfix_ast.empty()) {
				//if operand
				if(postfix_ast.front()->type == N_Var || postfix_ast.front()->type == N_Number || postfix_ast.front()->type == N_ListAcc) {
					operandStack.push(postfix_ast.front());
					postfix_ast.pop();
				}
				
				//if operator
				if(postfix_ast.front()->type == N_Plus) {
					//if tree queue is empty
					if(treeQueue.empty()) {
						postfix_ast.front()->init_plusNode(nullptr, operandStack.top());
						operandStack.pop();
						postfix_ast.front()->left = operandStack.top();
						operandStack.pop();
						treeQueue.push(postfix_ast.front());
					}
					
					//if tree queue is not empty
					else {
						postfix_ast.front()->init_plusNode(operandStack.top(), treeQueue.front());
						operandStack.pop();
						treeQueue.pop();
						treeQueue.push(postfix_ast.front());
					}
					
					postfix_ast.pop();
				}
			}
			
			//check if tree queue has size 1, if so return tree else return false node
			if(treeQueue.size() == 1) {
				nextToken();
				return treeQueue.front();
			} else {
				for(ASTNode* n: nodeExprLst)
					delete n;
				if(!treeQueue.empty()) {
					while(!treeQueue.empty()) {
						delete treeQueue.front();
						treeQueue.pop();
					}
				}
				
				delete resExpr_ast;
				delete leftOp_ast;
			}
			
			ASTNode* failNode_ast = new ASTNode(N_NILNode);
			setCurrTokIndex(backTrackIdx);
			return failNode_ast;
		}
		
		//function for list ::= "[" atom ("," atom)* "]" | "[" "]"
		ASTNode* getList(int backTrackIdx) {
			vector<string> list_ast;
			if(currTok.token_type == T_OpenBracket) {
				nextToken(); //should be either atom or closded bracket
				
				//empty list
				if(currTok.token_type == T_CloseBracket) {
					ASTNode* lstNode_ast = new ASTNode(N_List, currTok.tok_lineNum);
					lstNode_ast->init_listNode(list_ast);
					nextToken();
					return lstNode_ast;
				}
				
				//not empty list
				if(currTok.token_type == T_INT || currTok.token_type == T_Identifier) {
					if(currTok.token_type == T_INT) {
						list_ast.push_back(currTok.token_value);
					} else if(currTok.token_type == T_Identifier) {
						if(symbolTable.find(currTok.token_value) != symbolTable.end()) {
							if(symbolTable[currTok.token_value].second == INT) {
								list_ast.push_back(symbolTable[currTok.token_value].first);
							} else {
								//raise invalid type error
								string errMsg = ", lists may only contain ints or int variables, multiple dimensions are not supported";
								raiseSyntaxError(errMsg, currTok.tok_lineNum, RunTimeError);
							}
						} else {
							//raise error
							string errMsg = ", \'" + currTok.token_value + "\' not defined";
							raiseSyntaxError(errMsg, currTok.tok_lineNum, RunTimeError);
						}
					}
					
					nextToken(); //should be comma or close bracket
					while(currTok.token_type != T_CloseBracket && currTok.token_type != T_NONE) {
						if(currTok.token_type == T_Comma) {
							nextToken(); //should be INT|IDENTIFIER
							if(currTok.token_type == T_INT || currTok.token_type == T_Identifier) {
								if(currTok.token_type == T_INT) {
									list_ast.push_back(currTok.token_value);
								} else if(currTok.token_type == T_Identifier) {
									if(symbolTable.find(currTok.token_value) != symbolTable.end()) {
										if(symbolTable[currTok.token_value].second == INT) {
											list_ast.push_back(symbolTable[currTok.token_value].first);
										} else {
											//raise invalid type error
											string errMsg = ", lists may only contain ints or int variables, multiple dimensions are not supported";
											raiseSyntaxError(errMsg, currTok.tok_lineNum, RunTimeError);
										}
									} else {
										//raise error
										string errMsg = ", \'" + currTok.token_value + "\' not defined";
										raiseSyntaxError(errMsg, currTok.tok_lineNum, RunTimeError);
									}
								}
							}
						} else {
							//raise error
							raiseSyntaxError("','", currTok.tok_lineNum);
						}
						
						nextToken();
					}
					
					//if list is complete return node
					if(currTok.token_type == T_CloseBracket) {
						ASTNode* lstNode_ast = new ASTNode(N_List, currTok.tok_lineNum);
						lstNode_ast->init_listNode(list_ast);
						nextToken();
						return lstNode_ast;
					} else {
						//raise error
						raiseSyntaxError("']'", currTok.tok_lineNum);
					}
				} else {
					//raise error
					raiseSyntaxError("integer or variable", currTok.tok_lineNum);
				}
			}
			
			ASTNode* failNode_ast = new ASTNode(N_NILNode);
			setCurrTokIndex(backTrackIdx);
			return failNode_ast;
		}
		
		//function for assign ::= (IDENTIFIER "=" expr | list | list_splice) | (list_acc "=" expr) | (list_splice "=" list_splice)
		ASTNode* assign(int backTrackIdx) {
			ASTNode* toBeAssignNode_ast = list_acc(tok_idx);
			ASTNode* rightHandSideNode_ast = nullptr;
			ASTNode* assignNode_ast = nullptr;
			
			//is list access
			if(toBeAssignNode_ast->type != N_NILNode) {
				//nextToken(); //should be "="
				if(currTok.token_type == T_EQ) {
					nextToken(); //should be expr
					
					rightHandSideNode_ast = expr(tok_idx);
					if(rightHandSideNode_ast->type != N_NILNode) {
						//assign
						assignNode_ast = new ASTNode(N_Assign, currTok.tok_lineNum);
						assignNode_ast->init_assignNode(toBeAssignNode_ast, rightHandSideNode_ast);
						nextToken();
						return assignNode_ast;
					} else {
						//raise error
						delete toBeAssignNode_ast;
						delete rightHandSideNode_ast;
						delete assignNode_ast;
						raiseSyntaxError("expression", currTok.tok_lineNum);
					}
				} else {
					//raise error
					raiseSyntaxError("=", currTok.tok_lineNum);
				}
			} 
			
			//checking for list splice
			if(toBeAssignNode_ast->type == N_NILNode) {
				delete toBeAssignNode_ast;
				toBeAssignNode_ast = list_splice(tok_idx);
			}
			
			//is list splice
			if(toBeAssignNode_ast->type == N_List_Splice) {
				//nextToken() already called
				if(currTok.token_type == T_EQ) {
					nextToken(); //should be list splice
					rightHandSideNode_ast = list_splice(tok_idx);
					
					if(rightHandSideNode_ast->type == N_NILNode) {
						//raise error
						delete toBeAssignNode_ast;
						delete rightHandSideNode_ast;
						delete assignNode_ast;
						raiseSyntaxError("list splice", currTok.tok_lineNum);
					} else {
						assignNode_ast = new ASTNode(N_Assign, currTok.tok_lineNum);
						assignNode_ast->init_assignNode(toBeAssignNode_ast, rightHandSideNode_ast);
						nextToken();
						return assignNode_ast;
					}
				} else {
					//raise error
					delete toBeAssignNode_ast;
					delete rightHandSideNode_ast;
					delete assignNode_ast;
					raiseSyntaxError("'='", currTok.tok_lineNum);
				}
			}
			
			//is not list access or splice / is identifier
			if(toBeAssignNode_ast->type == N_NILNode) {
				delete toBeAssignNode_ast;
				if(currTok.token_type == T_Identifier) {
					toBeAssignNode_ast = new ASTNode(N_Var, currTok.tok_lineNum);
					toBeAssignNode_ast->init_varNode(currTok.token_value, D_NIL, nullptr);
					nextToken(); //should be "eq"
					
					if(currTok.token_type == T_EQ) {
						nextToken(); //should be either expr or list
						//if list
						if(currTok.token_type == T_OpenBracket) {
							toBeAssignNode_ast->dataType = LIST;
							rightHandSideNode_ast = getList(tok_idx);
							
							if(rightHandSideNode_ast->type != N_NILNode) {
								//assign
								assignNode_ast = new ASTNode(N_Assign, currTok.tok_lineNum);
								assignNode_ast->init_assignNode(toBeAssignNode_ast, rightHandSideNode_ast);
								nextToken();
								return assignNode_ast;
							} else {
								//raise error
								delete toBeAssignNode_ast;
								delete rightHandSideNode_ast;
								delete assignNode_ast;
								raiseSyntaxError("list", currTok.tok_lineNum);
							}
						}
						
						//if not list
						else {
							rightHandSideNode_ast = list_splice(tok_idx);
							if(rightHandSideNode_ast->type == N_NILNode) {
								delete rightHandSideNode_ast;
							} else if(rightHandSideNode_ast->type == N_List_Splice) {
								assignNode_ast = new ASTNode(N_Assign, currTok.tok_lineNum);
								assignNode_ast->init_assignNode(toBeAssignNode_ast, rightHandSideNode_ast);
								nextToken();
								return assignNode_ast;
							}
							
							rightHandSideNode_ast = expr(tok_idx);
							if(rightHandSideNode_ast->type != N_NILNode) {
								//assign
								assignNode_ast = new ASTNode(N_Assign, currTok.tok_lineNum);
								assignNode_ast->init_assignNode(toBeAssignNode_ast, rightHandSideNode_ast);
								nextToken();
								return assignNode_ast;
							} else {
								//raise error
								raiseSyntaxError("expression or list splice", currTok.tok_lineNum);
							}
						}
					} else {
						//raise error
						raiseSyntaxError("=", currTok.tok_lineNum);
					}
					
				} else {
					//raise error
					raiseSyntaxError("expression or list", currTok.tok_lineNum);
				}
			}
			
			ASTNode* failNode_ast = new ASTNode(N_NILNode);
			setCurrTokIndex(backTrackIdx);
			return failNode_ast;
		}
		
		//function for print ::= "print" "(" (atom|list_acc) ")" || "print" "(" str_lit "," (atom|list_acc) ")"
		ASTNode* printOneOrTwo(int backTrackIdx) {
			ASTNode* printNode_ast = nullptr;
			
			if(currTok.token_value == "print") {
				nextToken(); //should be "("
				if(currTok.token_type == T_OpenParen) {
					nextToken(); //should be either string literal or (atom|list_acc)
				
					//string literal
					// "print" "(" str_lit "," (atom|list_acc) ")"
					if(currTok.token_type == T_String_Literal) {
						ASTNode* strLit_ast = new ASTNode(N_StrLtr, currTok.tok_lineNum);
						strLit_ast->init_strLtrNode(currTok.token_value);
						
						nextToken(); //should be comma
						if(currTok.token_type == T_Comma) {
							nextToken(); //should be (atom|list_acc)
							
							ASTNode* printValNode_ast = atom(tok_idx);
							if(printValNode_ast->type == N_NILNode) {
								delete printValNode_ast;
								printValNode_ast = list_acc(tok_idx);
							}
							
							if(printValNode_ast->type == N_NILNode) {
								delete printNode_ast;
								delete strLit_ast;
								delete printValNode_ast;
								raiseSyntaxError("identifier, number or list access", currTok.tok_lineNum);
							}
							
							//next token called inside atom() and/or list_acc()
							if(currTok.token_type == T_CloseParen) {
								//return true
								printNode_ast = new ASTNode(N_Print2, currTok.tok_lineNum);
								printNode_ast->init_printTwo(strLit_ast, printValNode_ast);
								nextToken();
								return printNode_ast;
							} else {
								//raise error
								delete printNode_ast;
								delete strLit_ast;
								delete printValNode_ast;
								raiseSyntaxError("')'", currTok.tok_lineNum);
							}
							
						} else {
							//raise error
							delete printNode_ast;
							delete strLit_ast;
							raiseSyntaxError("','", currTok.tok_lineNum);
						}
					}
					
					//if not string literal
					else {
						ASTNode* printValNode_ast = atom(tok_idx);
						if(printValNode_ast->type == N_NILNode) {
							delete printValNode_ast;
							printValNode_ast = list_acc(tok_idx);
						}
						
						if(printValNode_ast->type == N_NILNode) {
							delete printNode_ast;
							delete printValNode_ast;
							raiseSyntaxError("identifier, number or list access", currTok.tok_lineNum);
						}
						
						//next token called inside atom() and/or list_acc()
						if(currTok.token_type == T_CloseParen) {
							//return true
							printNode_ast = new ASTNode(N_Print1, currTok.tok_lineNum);
							printNode_ast->init_printOne(printValNode_ast);
							nextToken();
							return printNode_ast;
						} else {
							//raise error
							delete printNode_ast;
							raiseSyntaxError("')'", currTok.tok_lineNum);
						}
					}
				} else {
					//raise error
					delete printNode_ast;
					raiseSyntaxError("'('", currTok.tok_lineNum);
				}
			}
			
			delete printNode_ast;
			ASTNode* failNode_ast = new ASTNode(N_NILNode);
			setCurrTokIndex(backTrackIdx);
			return failNode_ast;
		}
		
		//if statement
};

#endif