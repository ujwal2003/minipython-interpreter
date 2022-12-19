#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <iostream>
#include <vector>
#include <map>
#include <utility>
#include <stack>
#include "ast.h"
#include "global_scope.h"
#include "error.h"

class Interpreter {
	private:
		ASTNode* root;
		vector<ASTNode*> codeBlock;
		bool blockFlag = false;
		
	public:
		//initialization method (for one tree)
		void initialize(ASTNode* tree) {
			emptyEvalTracker();
			root = tree;
			blockFlag = false;
		}
		
		//initialization method (for block of code)
		void initialize(vector<ASTNode*> block) {
			emptyEvalTracker();
			codeBlock = block;
			root = nullptr;
			blockFlag = true;
		}
		
		//error
		void raiseRunTimeError(string errorMsg, int lineNumber) {
			if(!blockFlag) {
				deleteAST(root);
			} else if(blockFlag) {
				for(ASTNode* n: codeBlock) {
					deleteAST(n);
				}
			}
			
			RaiseError(RunTimeError, errorMsg, lineNumber);
		}
		
		//code evaluation
		void CodeEval(ASTNode* node) {
			//none
			if(node == nullptr) {
				evalHolder temp;
				temp.dat = D_NIL;
				temp.val = "NULL";
				evalTracker.push(temp);
				return;
			}
			//number node
			if(node->type == N_Number) {
				evalHolder temp;
				temp.dat = INT;
				temp.val = node->nodeVal;
				evalTracker.push(temp);
				return;
			}
			//list node
			if(node->type == N_List) {
				evalHolder temp;
				temp.dat = LIST;
				vector<string> tempLst = node->listVal;
				temp.listVal = tempLst;
				evalTracker.push(temp);
				return;
			}
			//string literal node
			if(node->type == N_StrLtr) {
				evalHolder temp;
				temp.dat = STR_LITERAL;
				temp.val = node->nodeVal;
				evalTracker.push(temp);
				return;
			}
			//list access node
			if(node->type == N_ListAcc) {
				CodeEval(node->left); //get list name
				evalHolder lstVar = evalTracker.top();
				evalTracker.pop();
				
				CodeEval(node->right); //get index number
				evalHolder lstIdx = evalTracker.top();
				evalTracker.pop();
				
				if(lstVar.dat == LIST && lstIdx.dat == INT) {
					evalHolder lstAccVal;
					lstAccVal.dat = INT;
					int idx = stoi(lstIdx.val);
					vector<string> tempVec = lstVar.listVal;
					
					if(idx > tempVec.size()) {
						raiseRunTimeError(", index out of bounds", node->lineNum);
					}
					
					string resVal = tempVec[idx];
					lstAccVal.val = resVal;
					evalTracker.push(lstAccVal);
					return;
				} else {
					//raise error
					raiseRunTimeError(", could not execute code for list access", node->lineNum);
				}
			}
			//list splice node
			if(node->type == N_List_Splice) {
				CodeEval(node->left); //get var node
				evalHolder lstVarName = evalTracker.top();
				evalTracker.pop();
				
				CodeEval(node->right); //get splice number
				evalHolder spliceValue_str = evalTracker.top();
				evalTracker.pop();
				
				bool isSpliceVal = true;
				int spliceVal;
				if(spliceValue_str.dat == INT) {
					spliceVal = stoi(spliceValue_str.val);
					isSpliceVal = true;
				} else if(spliceValue_str.dat == D_NIL && spliceValue_str.val == "NULL") {
					isSpliceVal = false;
				} else {
					//raise error
					raiseRunTimeError(", invalid type", node->lineNum);
				}
				
				evalHolder returnVal;
				vector<string> returnList;
				
				if(lstVarName.dat == LIST) {
					vector<string> origList = lstVarName.listVal;
					if(isSpliceVal && (spliceVal > origList.size())) {
						//raise error
						raiseRunTimeError(", index out of bounds", node->lineNum);
					}
					
					if(node->nodeVal == "T") {
						for(int i=spliceVal; i<origList.size(); i++) {
							returnList.push_back(origList[i]);
						}
						returnVal.dat = LIST;
						returnVal.listVal = returnList;
						evalTracker.push(returnVal);
						return;
					} else if(node->nodeVal == "F") {
						returnList = origList;
						returnVal.dat = LIST;
						returnVal.listVal = returnList;
						evalTracker.push(returnVal);
						return;
					}
				} else {
					//raise error
					raiseRunTimeError(", invalid type", node->lineNum);
				}
			}
			//var node
			if(node->type == N_Var) {
				string varName = node->nodeVal;
				if(symbolTable.find(varName) != symbolTable.end()) {
					if(symbolTable[varName].second == LIST) {
						if(listSymbolTable.find(varName) != listSymbolTable.end()) {
							//place list data in stack
							evalHolder temp;
							temp.dat = LIST;
							temp.listVal = listSymbolTable[varName];
							evalTracker.push(temp);
							return;
						} else {
							//raise error
							string errMsg = ", \'" + varName + "\' is not defined"; 
							raiseRunTimeError(errMsg, node->lineNum);
						}
					} else {
						//regular variable
						if(symbolTable[varName].second == INT) {
							//place variable data in stack
							evalHolder temp;
							temp.dat = INT;
							temp.val = symbolTable[varName].first;
							evalTracker.push(temp);
							return;
						} else {
							//raise error
							string errMsg = ", could not fetch \'" + varName + "\' from symbol table"; 
							raiseRunTimeError(errMsg, node->lineNum);
						}
					}
				} else {
					//raise error
					string errMsg = ", \'" + varName + "\' is not defined"; 
					raiseRunTimeError(errMsg, node->lineNum);
				}
				
				//just in case
				raiseRunTimeError(", error fetching variable", node->lineNum);
			}
			//assign node
			if(node->type == N_Assign) {
				//variable
				if(node->left->type == N_Var) {
					string varName = node->left->nodeVal;
					
					CodeEval(node->right); //get right value
					evalHolder varVal = evalTracker.top();
					evalTracker.pop();
					
					if(varVal.dat == INT) {
						pair<string, DataType> symTabVarVal(varVal.val, INT);
						symbolTable[varName] = symTabVarVal;
						return;
						
					} else if(varVal.dat == LIST) {
						vector<string> tempLst = varVal.listVal;
						pair<string, DataType> lstSymTabDat("", LIST);
						symbolTable[varName] = lstSymTabDat;
						listSymbolTable[varName] = tempLst;
						return;
						
					} else {
						//raise error
						string errMsg = ", failed to allocate data for \'" + varName + "\' in symbol table";
						raiseRunTimeError(errMsg, node->lineNum);
					}
				}
				//list access
				else if(node->left->type == N_ListAcc) {
					string lstVarName = node->left->left->nodeVal;
					int idxNum = stoi(node->left->right->nodeVal);
					vector<string> tempLst;
					
					//check if list exists
					if(symbolTable.find(lstVarName) != symbolTable.end()) {
						if(symbolTable[lstVarName].second == LIST) {
							if(listSymbolTable.find(lstVarName) != listSymbolTable.end()) {
								tempLst = listSymbolTable[lstVarName];
							} else {
								string errMsg = ", could not fetch \'" + lstVarName + "\' from symbol table";
								raiseRunTimeError(errMsg, node->lineNum);
							}
						} else {
							//raise error
							string errMsg = ", \'" + lstVarName + "\' is not defined"; 
							raiseRunTimeError(errMsg, node->lineNum);
						}
					} else {
						//raise error
						string errMsg = ", \'" + lstVarName + "\' is not defined"; 
						raiseRunTimeError(errMsg, node->lineNum);
					}
					
					//check if index is not out of bounds
					if(idxNum > tempLst.size()) {
						//raise error
						raiseRunTimeError(", index out of bounds", node->lineNum);
					}
					
					CodeEval(node->right); //get right value
					evalHolder tempVarVal = evalTracker.top();
					evalTracker.pop();
					
					if(tempVarVal.dat == INT) {
						tempLst[idxNum] = tempVarVal.val;
						listSymbolTable[lstVarName] = tempLst;
						return;
					} else {
						//raise error, this interpreter does not hanlde 2d lists
						raiseRunTimeError(", this interpreter does not handle 2d lists", node->lineNum);
					}
				}
				//list splice
				else if(node->left->type == N_List_Splice) {
					CodeEval(node->left);
					evalHolder leftHandSide = evalTracker.top();
					evalTracker.pop();
					if(leftHandSide.dat != LIST) {
						//raise error
						raiseRunTimeError(", invalid types", node->lineNum);
					}
					
					string leftSideVarName = node->left->left->nodeVal;
					CodeEval(node->left->right);
					evalHolder s_leftSpliceIdx = evalTracker.top();
					evalTracker.pop();
					if(s_leftSpliceIdx.dat != INT) {
						//raise error
						raiseRunTimeError(", invalid types", node->lineNum);
					}
					int leftSpliceIdx = stoi(s_leftSpliceIdx.val);
					
					if(node->right->type != N_List_Splice) {
						//raise error
						raiseRunTimeError(", expected list splice", node->lineNum);
					}
					
					CodeEval(node->right);
					evalHolder rightHandSide = evalTracker.top();
					evalTracker.pop();
					
					if(rightHandSide.dat != LIST) {
						//raise error
						raiseRunTimeError(", invalid types", node->lineNum);
					}
					
					vector<string> origLeftHandVector = listSymbolTable[leftSideVarName];
					if(leftSpliceIdx > origLeftHandVector.size()) {
						//raise error
						raiseRunTimeError(", index out of bounds", node->lineNum);
					}
					
					vector<string> leftHandSplicedVector = leftHandSide.listVal;
					vector<string> rightHandSplicedVector = rightHandSide.listVal;
					
					leftHandSplicedVector = rightHandSplicedVector;
					origLeftHandVector.erase(origLeftHandVector.begin()+(leftSpliceIdx-1), origLeftHandVector.end());
					origLeftHandVector.insert(origLeftHandVector.end(), leftHandSplicedVector.begin(), leftHandSplicedVector.end());
					listSymbolTable[leftSideVarName] = origLeftHandVector;
					return;
				}
				//error
				else {
					//raise error
					raiseRunTimeError(", invalid type assignment", node->lineNum);
				}
			}
			//plus node
			if(node->type == N_Plus) {
				CodeEval(node->left); //get left operand
				evalHolder leftOp = evalTracker.top();
				evalTracker.pop();
				
				CodeEval(node->right); //get right operand
				evalHolder rightOp = evalTracker.top();
				evalTracker.pop();
				
				if(leftOp.dat == INT && rightOp.dat == INT) {
					//do addition
					int temp1 = stoi(leftOp.val);
					int temp2 = stoi(rightOp.val);
					int temp3 = temp1 + temp2;
					
					evalHolder addRes;
					addRes.dat = INT;
					addRes.val = to_string(temp3);
					evalTracker.push(addRes);
				} else if(leftOp.dat == LIST && rightOp.dat == LIST) {
					//concatenate list
					vector<string> tempLst1 = leftOp.listVal;
					vector<string> tempLst2 = rightOp.listVal;
					vector<string> resLst = leftOp.listVal;
					resLst.insert(resLst.end(), tempLst2.begin(), tempLst2.end());
					
					evalHolder concatRes;
					concatRes.dat = LIST;
					concatRes.listVal = resLst;
					evalTracker.push(concatRes);
				} else {
					//raise type error
					raiseRunTimeError(", invalid types", node->lineNum);
				}
				
				return;
			}
			//print(one_arg) node
			if(node->type == N_Print1) {
				CodeEval(node->child); //visit child
				evalHolder temp = evalTracker.top(); //get child
				
				if(temp.dat == INT) {
					cout << temp.val << endl;
				} else if(temp.dat == LIST) {
					cout << '[' << stringVector(temp.listVal) << ']' << endl;
				}
				
				evalTracker.pop();
				return;
			}
			//print(two_args) node
			if(node->type == N_Print2) {
				CodeEval(node->left); //get string literal
				evalHolder strLit_val = evalTracker.top();
				string printStrLit = "";
				if(strLit_val.dat == STR_LITERAL) {
					printStrLit = strLit_val.val;
				} else {
					//raise error
					raiseRunTimeError("not string literal", node->lineNum);
				}
				evalTracker.pop();
				
				CodeEval(node->right); //get var or int val
				evalHolder otherVal = evalTracker.top();
				if(otherVal.dat == LIST) {
					cout << printStrLit << ' ';
					cout << '[' << stringVector(otherVal.listVal) << ']' << endl;
				} else {
					//regular variable
					cout << printStrLit << ' ';
					cout << otherVal.val << endl;
				}
				evalTracker.pop();
				return;
			}
			//error
			else {
				raiseRunTimeError(", unknown error, could not execute program.", node->lineNum);
			}
		}
		
		//evaluate code or code block
		void evaluate() {
			if(!blockFlag) {
				CodeEval(root);
				deleteAST(root);
			} 
			
			else if(blockFlag) {
				for(ASTNode* node: codeBlock) {
					CodeEval(node);
					deleteAST(node);
				}
			}
		}
		
		//empty stack
		void emptyEvalTracker() {
			while(!evalTracker.empty()) {
				evalTracker.pop();
			}
		}
		
		//vector<string> as string
		string stringVector(vector<string> v) {
			string str;
			for(int i=0; i<v.size(); i++) {
				if(i != 0)
					str += ", ";
				str += v[i];
			}
			return str;
		}
};

#endif