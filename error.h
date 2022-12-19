#ifndef ERROR_H
#define ERROR_H

#include <iostream>
using namespace std;

enum ErrType {
	InvalidCharacterError,
	InvalidSyntaxError,
	RunTimeError,
	DefaultError
};

struct CreateProgramError : public exception {
	const char * what() const throw () {
		return "Error encountered, program stopped.";
	}
};

void RaiseError(ErrType err, string txt, int lineNum) {
	switch(err) {
		case InvalidCharacterError: {
			cout << "InvalidCharacterError --> \'" << txt << "\' at line " << lineNum << ". ";
			throw CreateProgramError();
		}
		
		case InvalidSyntaxError: {
			cout << "InvalidSyntaxError at line " << lineNum << ", expected " << txt << ". ";
			throw CreateProgramError();
		}
		
		case RunTimeError: {
			cout << "RunTimeError at line " << lineNum << txt << ". ";
			throw CreateProgramError();
		}
		
		default: {
			cout << "Error at line " << lineNum << " --> " << txt << ". ";
			throw CreateProgramError();
		}
	}
}
#endif