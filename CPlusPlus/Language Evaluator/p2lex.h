#ifndef P2LEX_H
#define P2LEX_H

// header file for program 2 (and beyond)
// lexical analyzer

#include <iostream>
#include <string>
using namespace std;

enum Token {
    ID,		// an identifier
    
    STRING,		// a string enclosed in ""
    
    UNION,		// the + operator
    INTERSECT,	// the ^ operator
    
    SET,		// the set keyword
    PRINT,		// the print keyword
    SEARCH,		// the search keyword
    FOR,		// the for keyword
    
    LPAREN,		// left parenthesis
    RPAREN,		// right parenthesis
    
    SC,		// the semicolon
    
    ERR,		// some error condition was reached
    DONE,		// end of file
};

extern Token getToken(istream *br, string& lexeme);
extern bool pushbackToken(Token t, string lexeme);

extern int linenum;

#endif