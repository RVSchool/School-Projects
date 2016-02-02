//
//  p3parser.cpp
//  parser
//

#include <string>
#include <iostream>
using namespace std;

#include "p2lex.h"
#include "p3parser.h"

int errcnt = 0;
int linenum = 2;

void
errmsg(string msg)
{
    ++errcnt;
    cerr << linenum << ": " << msg << endl;
}

string
stripQuotes(string& s) {
    return s.substr(1, s.length()-2);
}

//Program ::= StmtList
//StmtList ::= Stmt | Stmt StmtList
//Stmt ::= PRINT Expr SC | SET ID Expr SC
//Expr ::= Term UNION Expr | Term
//Term ::= Primary INTERSECT Term | Primary
//Primary ::= Search_expr | ID | STRING | LPAREN Expr RPAREN
//Search_expr ::= SEARCH STRING FOR STRING

// prototypes for the functions
extern PTree *Program(istream *br);
extern PTree *StmtList(istream *br);
extern PTree *Stmt(istream *br);
extern PTree *Expr(istream *br);
extern PTree *Term(istream *br);
extern PTree *Primary(istream *br);

// Program ::= StmtList
PTree *Program(istream *br)
{
    PTree *sl = StmtList(br);
    
    Token t;
    string s;
    t = getToken(br, s);
    if( t != DONE )
        return 0;
    
    return sl;
}

// StmtList ::= Stmt | Stmt StmtList
PTree *StmtList(istream *br)
{
    PTree *stmt;
    
    stmt = Stmt(br);
    
    if( stmt )
        return new PTreeStmtList( linenum, stmt, StmtList(br) );
    return 0;
}

// Stmt ::= PRINT Expr SC | SET ID Expr SC
PTree *Stmt(istream *br)
{
    Token t;
    string lexeme;
    
    t = getToken(br, lexeme);
    int line = linenum;
    
    if( t == PRINT ) {
        PTree *expr = Expr(br);
        if( expr == 0 ) {
            errmsg("Expected an expression");
            return 0;
        }
        
        t = getToken(br, lexeme);
        if( t != SC ) {
            errmsg("Missing semicolon");
            return 0;
        }
        
        return new PTreePrint(line, expr);
    }
    else if( t == SET ) {
        string id;
        t = getToken(br, id);
        if( t != ID ) {
            errmsg("Expecting identifier");
            return 0;
        }
        
        PTree *expr = Expr(br);
        if( expr == 0 ) {
            errmsg("Expected an expression");
            return 0;
        }
        
        t = getToken(br, lexeme);
        if( t != SC ) {
            errmsg("Missing semicolon");
            return 0;
        }
        
        return new PTreeSet(line, id, expr);
    }
    else if( t != DONE )
        errmsg("not a valid statement");
    
    return 0;
}

// Expr ::= Term UNION Expr | Term
// or  Expr ::= Term { UNION Expr }
PTree *Expr(istream *br)
{
    PTree *term = Term(br);
    if( term == 0 ) return 0;
    
    Token t;
    string lexeme;
    
    t = getToken(br, lexeme);
    int line = linenum;
    
    if( t != UNION ) {
        pushbackToken(t,lexeme);
        return term;
    }
    
    PTree *expr = Expr(br);
    if( expr == 0 ) {
        errmsg("Expected an expression");
        return 0;
    }
    
    return new PTreeUnion(line,term,expr);
}

// Term ::= Primary INTERSECT Term | Primary
// or  Term ::= Primary { INTERSECT Term }
PTree *Term(istream *br)
{
    PTree *pri = Primary(br);
    if( pri == 0 ) return 0;
    
    Token t;
    string lexeme;
    
    t = getToken(br, lexeme);
    int line = linenum;
    
    if( t != INTERSECT ) {
        pushbackToken(t,lexeme);
        return pri;
    }
    
    PTree *term = Term(br);
    if( term == 0 ) {
        errmsg("Expected an expression");
        return 0;
    }
    
    return new PTreeIntersect(line,pri,term);
}

//Primary ::= Search_expr | ID | STRING | LPAREN Expr RPAREN
//Search_expr ::= SEARCH STRING FOR STRING
PTree *Primary(istream *br)
{
    Token t;
    string lexeme;
    
    t = getToken(br, lexeme);
    int line = linenum;
    
    if( t == ID ) {
        return new PTreeID(line, lexeme);
    }
    else if( t == STRING ) {
        return new PTreeSTR(line, stripQuotes(lexeme));
    }
    else if( t == LPAREN ) {
        PTree *e = Expr(br);
        if ( e ) {
            t = getToken(br, lexeme);
            if( t == RPAREN ) {
                return e;
            }
            else {
                errmsg("Missing right paren");
                return 0;
            }
        }
        else {
            errmsg("Missing expression after left paren");
            return 0;
        }
    }
    else if( t == SEARCH ) {
        string searchin, ident;
        if( (t = getToken(br, searchin)) != STRING ) {
            errmsg("expecting STRING after SEARCH");
            return 0;
        }
        if ((t = getToken(br, ident)) != FOR ) {
            errmsg("expecting FOR");
            return 0;
        }
        if ((t = getToken(br, ident)) != STRING ) {
            errmsg("expecting STRING after FOR");
            return 0;
        }
        
        return new PTreeSearch(line, stripQuotes(searchin), stripQuotes(ident));
    }
    
    errmsg("Missing primary");
    return 0;
}