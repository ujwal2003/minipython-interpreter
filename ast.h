#ifndef AST_H
#define AST_H

#include <iostream>
#include <vector>
#include <queue>

using namespace std;

//node types enum
enum NodeType {
	N_Assign, N_Plus,
	N_Var, N_Number, 
	N_List, N_ListAcc, N_List_Splice,
	N_Print1, N_Print2, N_StrLtr,
	N_ifStmt, N_BoolExpr,
	N_NILNode
};

//DataTypes enum
enum DataType {INT, LIST, STR_LITERAL, LIST_ACC, D_NIL};

//Abstract Syntax Tree class
class ASTNode {
	public:
		//to figure out nodetype
		NodeType type;
		int lineNum; //line number
		
		/*====Node Types====*/
		 
		// nodes that need left and right:
		//  assign, plus, print(two_arg),
		//  list_acc
		ASTNode* left;
		ASTNode* right;
		
		// nodes that need only one child:
		//  print(one_arg), varNode
		ASTNode* child;
		
		
		
		/*==end Node Types==*/
		
		/*====values====*/
		string nodeVal; //for number node or string literal node
		vector<string> listVal; //for list node
		DataType dataType; //for data types
		/*==end values==*/
		
		/*"constructors"*/
		
		//set node type
		ASTNode(NodeType nt, int inLineNum=-1) {
			type = nt;
			lineNum = inLineNum;
			
			//init
			left = nullptr;
			right = nullptr;
			nodeVal = "";
			dataType = D_NIL;
		}
		
		//var node
		void init_varNode(string varName, DataType inType, ASTNode* inNode) {
			nodeVal = varName;
			dataType = inType; //data type
			child = inNode; //variable value
			
			left = nullptr; //not using
			right = nullptr; //not using
		}
		//number node
		void init_numNode(string inVal) {
			nodeVal = inVal;
			
			child = nullptr; //not using
			left = nullptr; //not using
			right = nullptr; //not using
		}
		//list node
		void init_listNode(vector<string> inLst) {
			listVal = inLst;
			
			child = nullptr; //not using
			left = nullptr; //not using
			right = nullptr; //not using
		}
		//string literal node
		void init_strLtrNode(string str) {
			nodeVal = str;
			
			left = nullptr; //not using
			right = nullptr; //not using
			child = nullptr; //not using
		}
		//print(one_arg) node
		void init_printOne(ASTNode* inNode) {
			child = inNode;
			
			left = nullptr; //not using
			right = nullptr; //not using
		}
		//print(two_args) node
		void init_printTwo(ASTNode* inStrNode, ASTNode* inOtherNode) {
			left = inStrNode;
			right = inOtherNode;
			
			child = nullptr; //not using
		}
		//assign node
		void init_assignNode(ASTNode* inVarNode, ASTNode* inOtherNode) {
			left = inVarNode;
			right = inOtherNode;
			
			child = nullptr; //not using
		}
		//plus node
		void init_plusNode(ASTNode* leftOp, ASTNode* rightOp) {
			left = leftOp;
			right = rightOp;
			
			child = nullptr; //not using
		}
		//list access node
		void init_listAccessNode(ASTNode* varName, ASTNode* lstIndx) {
			left = varName;
			right = lstIndx;
			
			child = nullptr; //not using
		}
		//list splice node
		void init_listSpliceNode(ASTNode* varName, ASTNode* spliceIndx, string doSplice) {
			left = varName;
			right = spliceIndx;
			nodeVal = doSplice;
			
			child = nullptr; //not using
		}
		//if statement node
		void init_ifStmtNode(ASTNode* boolExprNode) {
			child = boolExprNode;
			
			left = nullptr; //not using
			right = nullptr; //not using
		}
		//bool expr node
		void init_boolExprNode(ASTNode* leftOp, string comparator, ASTNode* rightOp) {
			left = leftOp;
			nodeVal = comparator;
			right = rightOp;
			
			child = nullptr; //not using
		}
};

//delete AST method
//idk if this is the best way to delete this tree or not
void deleteAST(ASTNode* &root) {
	if(root == nullptr)
		return;
	
	if(root->left == root->right) {
		deleteAST(root->left);
		root->right = nullptr;
		deleteAST(root->child);
		return;
	}
	
	if(root->left == root->child) {
		deleteAST(root->left);
		root->child = nullptr;
		deleteAST(root->right);
		return;
	}
	
	if(root->right == root->child) {
		deleteAST(root->right);
		root->child = nullptr;
		deleteAST(root->left);
		return;
	}
	
	deleteAST(root->left);
	deleteAST(root->right);
	deleteAST(root->child);
	
	delete root;
	
	root = nullptr;
}
#endif