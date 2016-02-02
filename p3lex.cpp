// p3lex

#include "p2lex.h"
#include <cctype>

enum LexState {
    START,
    INID,
    INSTRING,
    INCOMMENT,
    ONESLASH,
};

inline Token
keywordCheck(const string& word)
{
    if (word == "set") {
        return SET;
    }
    else if (word == "print") {
        return PRINT;
    }
    else if (word == "search") {
        return SEARCH;
    }
    else if (word == "for") {
        return FOR;
    }
    return ID;
}

static Token
getActualToken(istream *br, string& lexeme)
{
    LexState state = START;
    lexeme.clear();
    
    int inchar;
    
    while ( (inchar = br->get()) != EOF && br->good() ) {
        if( inchar == '\n' )
            linenum++;
        
        switch (state) {
            case START:
                if (isspace(inchar)) {
                    continue;
                }
                
                lexeme += inchar;
                if (inchar == '+') {
                    return UNION;
                }
                else if (inchar == '^') {
                    return INTERSECT;
                }
                else if (inchar == '(') {
                    return LPAREN;
                }
                else if (inchar == ')') {
                    return RPAREN;
                }
                else if (inchar == ';') {
                    return SC;
                }
                
                else if (inchar == '/') {
                    state = ONESLASH;
                }
                else if (inchar == '"') {
                    state = INSTRING;
                }
                else if (isalpha(inchar)) {
                    state = INID;
                }
                else
                    return ERR;
                
                break;
                
            case INID:
                if (isalpha(inchar)) {
                    lexeme += inchar;
                    continue;
                }
                
                br->putback(inchar);
                return keywordCheck(lexeme);
                break;
                
            case INSTRING:
                lexeme += inchar;
                if (inchar == '\n') {
                    return ERR;
                }
                if (inchar != '"') {
                    continue;
                }
                
                return STRING;
                break;
                
            case INCOMMENT:
                if (inchar != '\n') {
                    continue;
                }
                state = START;
                break;
                
            case ONESLASH:
                if (inchar != '/') {
                    br->putback(inchar);
                    return ERR;
                }
                lexeme.clear();
                state = INCOMMENT;
                break;
                
            default:
                cerr << "This should never happen!" << endl;
                exit(1);
                break;
        }
    }
    
    // I reached an end of file... perhaps it happened in the middle of something...
    if (state == INID) {
        return keywordCheck(lexeme);
    }
    else if (state == ONESLASH || state == INSTRING) {
        return ERR;
    }
    return DONE;
}

// the real functions and pushback

bool		pushedBack = false;
static Token	pushedBackToken;
static string	pushedBackLexeme;
Token
getToken(istream *br, string& lexeme)
{
    if( pushedBack ) {
        pushedBack = false;
        lexeme = pushedBackLexeme;
        return pushedBackToken;
    }
    
    return getActualToken(br, lexeme);
}

bool
pushbackToken(Token t, string lexeme)
{
    if( !pushedBack ) {
        pushedBack = true;
        pushedBackToken = t;
        pushedBackLexeme = lexeme;
        return true;
    }
    
    return false;
}